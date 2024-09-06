#include "ReflectionCapture.h"

#include <filesystem>
#include <memory>

#include "Engine.h"
#include "TextureManager.h"

namespace Flame {
  void ReflectionCapture::Init() {
    diffusePipeline.Init(L"Assets/Shaders/iblDiffuse.hlsl", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    diffuseBuffer.Init();
    specularPipeline.Init(L"Assets/Shaders/iblSpecular.hlsl", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    specularBuffer.Init();
    reflectancePipeline.Init(L"Assets/Shaders/iblReflectance.hlsl", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    reflectanceBuffer.Init();

    {
      D3D11_SAMPLER_DESC desc {};
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
      desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
      desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
      desc.MaxLOD = D3D11_FLOAT32_MAX;
      desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
      HRESULT result = DxContext::Get()->d3d11Device->CreateSamplerState(&desc, m_linearSampler.GetAddressOf());
      assert(SUCCEEDED(result));
    }
  }

  void ReflectionCapture::Cleanup() {
    diffusePipeline.Reset();
    diffuseBuffer.Reset();
    specularPipeline.Reset();
    specularBuffer.Reset();
    reflectancePipeline.Reset();
    reflectanceBuffer.Reset();
    m_linearSampler.Reset();
  }

  void ReflectionCapture::GenerateAndSaveTextures() {
    ID3D11ShaderResourceView* skyTextureView = TextureManager::Get()->GetTexture(Engine::GetDirectory(L"Assets\\Textures\\lake_beach.dds"))->GetResourceView();
    auto diffuseTexture = GenerateDiffuseTexture(1000, 8, skyTextureView);
    auto specularTexture = GenerateSpecularTexture(1000, 1024, skyTextureView);
    auto reflectanceTexture = GenerateReflectanceTexture(1000, 512);

    std::filesystem::create_directories(Engine::GetDirectory(L"Generated\\Textures\\IBL"));
    TextureManager::SaveToDDS(
      Engine::GetDirectory(L"Generated\\Textures\\IBL\\diffuse.dds"),
      diffuseTexture->GetResource(),
      DXGI_FORMAT_R16G16B16A16_FLOAT,
      false
    );
    TextureManager::SaveToDDS(
      Engine::GetDirectory(L"Generated\\Textures\\IBL\\specular.dds"),
      specularTexture->GetResource(),
      DXGI_FORMAT_R16G16B16A16_FLOAT,
      false
    );
    TextureManager::SaveToDDS(
      Engine::GetDirectory(L"Generated\\Textures\\IBL\\reflectance.dds"),
      reflectanceTexture->GetResource(),
      DXGI_FORMAT_BC5_UNORM,
      false
    );
  }

  std::shared_ptr<Texture> ReflectionCapture::GenerateDiffuseTexture(uint32_t samples, uint32_t textureSize, ID3D11ShaderResourceView* skyboxView) {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;

    auto texture = CreateCubemap(textureSize, DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
    auto rtvArray = CreateCubemapRtv(texture->GetResource(), DXGI_FORMAT_R16G16B16A16_FLOAT, 0);
    diffuseBuffer.data.samples = samples;
    diffuseBuffer.data.cubemapSize = textureSize;

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
    dc->PSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    diffusePipeline.Bind();
    for (uint32_t i = 0; i < 6; ++i) {
      dc->OMSetRenderTargets(1, rtvArray[i].GetAddressOf(), nullptr);
      diffuseBuffer.data.normal = kCubemapFront[i];
      diffuseBuffer.ApplyChanges();
      dc->Draw(3, 0);
    }

    // Cleanup
    dc->OMSetRenderTargets(0, nullptr, nullptr);

    return texture;
  }

  std::shared_ptr<Texture> ReflectionCapture::GenerateSpecularTexture(uint32_t samples, uint32_t textureSize, ID3D11ShaderResourceView* skyboxView) {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;

    auto texture = CreateCubemap(textureSize, DXGI_FORMAT_R16G16B16A16_FLOAT, 0);
    specularBuffer.data.samples = samples;
    specularBuffer.data.cubemapSize = textureSize;
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->VSSetConstantBuffers(0, 1, specularBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, specularBuffer.GetAddressOf());
    dc->PSSetShaderResources(0, 1, &skyboxView);
    dc->PSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    specularPipeline.Bind();

    uint32_t mipLevels = static_cast<uint32_t>(std::ceilf(std::log2f(textureSize) + 1.0f));
    for (uint32_t mipLevel = 0; mipLevel < mipLevels; ++mipLevel) {
      uint32_t levelTextureSize = GetTextureSizeLevel(textureSize, mipLevel);

      auto rtvArray = CreateCubemapRtv(texture->GetResource(), DXGI_FORMAT_R16G16B16A16_FLOAT, mipLevel);
      // Render onto faces
      D3D11_VIEWPORT viewport {
        0.0f,
        0.0f,
        float(levelTextureSize),
        float(levelTextureSize),
        0.0f,
        1.0f,
      };
      dc->RSSetViewports(1, &viewport);

      specularBuffer.data.roughness = std::max(float(mipLevel) / float(mipLevels - 1), 0.045f);

      for (uint32_t i = 0; i < 6; ++i) {
        dc->OMSetRenderTargets(1, rtvArray[i].GetAddressOf(), nullptr);
        specularBuffer.data.normal = kCubemapFront[i];
        specularBuffer.ApplyChanges();
        dc->Draw(3, 0);
      }
    }

    // Cleanup
    dc->OMSetRenderTargets(0, nullptr, nullptr);

    return texture;
  }

  std::shared_ptr<Texture> ReflectionCapture::GenerateReflectanceTexture(uint32_t samples, uint32_t textureSize) {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;
    HRESULT result;
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> textureView;

    // Create texture
    {
      D3D11_TEXTURE2D_DESC desc {
        textureSize,
        textureSize,
        1,
        1,
        DXGI_FORMAT_R16G16_FLOAT,
        { 1, 0 },
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
        0,
        0
      };
      result = device->CreateTexture2D(&desc, nullptr, texture.ReleaseAndGetAddressOf());
      assert(SUCCEEDED(result));
    }

    // Create SRV
    {
      D3D11_SHADER_RESOURCE_VIEW_DESC desc {};
      desc.Format = DXGI_FORMAT_R16G16_FLOAT;
      desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      desc.Texture2D = { 0, 1 };
      result = device->CreateShaderResourceView(texture.Get(), &desc, textureView.ReleaseAndGetAddressOf());
      assert(SUCCEEDED(result));
    }

    // Create RTV
    ComPtr<ID3D11RenderTargetView> rtv;
    device->CreateRenderTargetView(texture.Get(), nullptr, rtv.ReleaseAndGetAddressOf());

    // Render
    D3D11_VIEWPORT viewport {
      0.0f,
      0.0f,
      float(textureSize),
      float(textureSize),
      0.0f,
      1.0f
    };
    dc->RSSetViewports(1, &viewport);
    reflectancePipeline.Bind();
    dc->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->PSSetConstantBuffers(0, 1, reflectanceBuffer.GetAddressOf());
    reflectanceBuffer.data.samples = samples;
    reflectanceBuffer.ApplyChanges();
    dc->Draw(3, 0);

    // Cleanup
    dc->OMSetRenderTargets(0, nullptr, nullptr);

    return std::make_shared<Texture>(std::move(texture), std::move(textureView));
  }

  std::shared_ptr<Texture> ReflectionCapture::CreateCubemap(uint32_t textureSize, DXGI_FORMAT format, uint32_t mipLevels) {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;
    HRESULT result;

    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> textureView;

    // Generate TextureCube
    D3D11_TEXTURE2D_DESC desc {
      textureSize,
      textureSize,
      mipLevels,
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

  std::array<ReflectionCapture::ComPtr<ID3D11RenderTargetView>, 6> ReflectionCapture::CreateCubemapRtv(ID3D11Resource* texture, DXGI_FORMAT format, uint32_t mipLevel) {
    auto device = DxContext::Get()->d3d11Device;
    HRESULT result;
    std::array<ComPtr<ID3D11RenderTargetView>, 6> rtvs;

    for (uint32_t i = 0; i < 6; ++i) {
      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc {};
      rtvDesc.Format = format;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
      rtvDesc.Texture2DArray.MipSlice = mipLevel;
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

  uint32_t ReflectionCapture::GetTextureSizeLevel(uint32_t size, uint32_t mipLevel) {
    if (mipLevel == 0) {
      return size;
    }

    if (size == 1) {
      return size;
    }

    return GetTextureSizeLevel(uint32_t(float(size) * 0.5f), mipLevel - 1);
  }
}
