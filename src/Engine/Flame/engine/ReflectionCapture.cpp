#include "ReflectionCapture.h"

#include <memory>

namespace Flame {
  void ReflectionCapture::Init() {
    diffusePipeline.Init(L"Assets/Shaders/iblDiffuse.hlsl", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    diffuseBuffer.Init();
    //specularPipeline.Init(L"Assets/Shaders/iblSpecular.hlsl", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    //specularBuffer.Init();
  }

  void ReflectionCapture::Cleanup() {
    diffusePipeline.Reset();
    diffuseBuffer.Reset();
    specularPipeline.Reset();
    specularBuffer.Reset();
  }

  std::shared_ptr<Texture> ReflectionCapture::GenerateDiffuseTexture(uint32_t textureSize, ID3D11ShaderResourceView* skyboxView) {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;

    auto texture = CreateCubemap(textureSize, DXGI_FORMAT_R16G16B16A16_FLOAT);
    auto rtvArray = CreateCubemapRtv(texture->GetResource(), DXGI_FORMAT_R16G16B16A16_FLOAT);
    auto transforms = GenerateTransformMatrices();

    // Render onto faces
    D3D11_VIEWPORT viewport {
      0.0f,
      0.0f,
      float(textureSize),
      float(textureSize),
      0.0f,
      1.0f,
    };
    dc->RSSetViewports(1, &viewport);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->VSSetConstantBuffers(0, 1, diffuseBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, diffuseBuffer.GetAddressOf());
    dc->PSSetShaderResources(0, 1, &skyboxView);
    diffusePipeline.Bind();
    for (uint32_t i = 0; i < 6; ++i) {
      dc->OMSetRenderTargets(1, rtvArray[i].GetAddressOf(), nullptr);
      diffuseBuffer.data.normal = kCubemapFront[i];
      diffuseBuffer.data.viewMatInv = transforms[i];
      diffuseBuffer.ApplyChanges();
      dc->Draw(3, 0);
    }

    return texture;
  }

  std::shared_ptr<Texture> ReflectionCapture::GenerateSpecularTexture(uint32_t textureSize, ID3D11ShaderResourceView* skyboxView) {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;

    auto texture = CreateCubemap(textureSize, DXGI_FORMAT_R16G16B16A16_FLOAT);
    auto rtvArray = CreateCubemapRtv(texture->GetResource(), DXGI_FORMAT_R16G16B16A16_FLOAT);
    auto transforms = GenerateTransformMatrices();

    // Render onto faces
    D3D11_VIEWPORT viewport {
      0.0f,
      0.0f,
      float(textureSize),
      float(textureSize),
      0.0f,
      1.0f,
    };
    dc->RSSetViewports(1, &viewport);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->VSSetConstantBuffers(0, 1, specularBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, specularBuffer.GetAddressOf());
    dc->PSSetShaderResources(0, 1, &skyboxView);
    specularPipeline.Bind();
    for (uint32_t i = 0; i < 6; ++i) {
      dc->OMSetRenderTargets(1, rtvArray[i].GetAddressOf(), nullptr);
      specularBuffer.data.normal = kCubemapFront[i];
      specularBuffer.data.viewMatInv = transforms[i];
      specularBuffer.ApplyChanges();
      dc->Draw(3, 0);
    }

    return texture;
  }

  std::shared_ptr<Texture> ReflectionCapture::CreateCubemap(uint32_t textureSize, DXGI_FORMAT format) {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;
    HRESULT result;

    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> textureView;

    // Generate TextureCube
    D3D11_TEXTURE2D_DESC desc {
      textureSize,
      textureSize,
      1,
      6,
      format,
      { 1, 0 },
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
      0,
      D3D11_RESOURCE_MISC_TEXTURECUBE
    };
    result = device->CreateTexture2D(
      &desc,
      nullptr,
      texture.ReleaseAndGetAddressOf()
    );
    assert(SUCCEEDED(result));

    // Create SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube = { 0, 1 };

    result = device->CreateShaderResourceView(
      texture.Get(),
      nullptr,
      textureView.ReleaseAndGetAddressOf()
    );
    assert(SUCCEEDED(result));

    return std::make_shared<Texture>(std::move(texture), std::move(textureView));
  }

  std::array<ReflectionCapture::ComPtr<ID3D11RenderTargetView>, 6> ReflectionCapture::CreateCubemapRtv(ID3D11Resource* texture, DXGI_FORMAT format) {
    auto device = DxContext::Get()->d3d11Device;
    HRESULT result;
    std::array<ComPtr<ID3D11RenderTargetView>, 6> rtvs;

    for (uint32_t i = 0; i < 6; ++i) {
      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc {};
      rtvDesc.Format = format;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
      rtvDesc.Texture2DArray.MipSlice = 0;
      rtvDesc.Texture2DArray.ArraySize = 1;
      rtvDesc.Texture2DArray.FirstArraySlice = i;

      result = device->CreateRenderTargetView(
        texture,
        &rtvDesc,
        rtvs[i].ReleaseAndGetAddressOf()
      );
      assert(SUCCEEDED(result));
    }

    return rtvs;
  }

  std::array<glm::mat4, 6> ReflectionCapture::GenerateTransformMatrices() {
    std::array<glm::mat4, 6> matrices {};
    for (uint32_t i = 0; i < 6; ++i) {
      // It's inversed since glm matrices are column major
      matrices[i] = glm::transpose(glm::mat4(
        kCubemapRight[i], kCubemapUp[i], kCubemapFront[i], { 0, 0, 0, 1 }
      ));
    }

    return matrices;
  }
}
