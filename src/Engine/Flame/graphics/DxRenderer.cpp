#include "DxRenderer.h"
#include "Flame/engine/LightSystem.h"
#include "Flame/engine/MeshSystem.h"
#include "Flame/math/HitRecord.h"
#include "Flame/utils/PtrProxy.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <winnt.h>
#include <Flame/engine/Engine.h>
#include <Flame/engine/TextureManager.h>

#include "Flame/utils/draggers/IDragger.h"

#include "Flame/utils/Random.h"
#include "Flame/math/MathUtils.h"
#include "PostProcess.h"
#include "glm/ext.hpp"

namespace Flame {
  DxRenderer::DxRenderer(std::shared_ptr<Window> window, std::shared_ptr<AlignedCamera> camera)
  : m_window(std::move(window))
  , m_camera(std::move(camera)) {
    m_input = &m_window->GetInputSystem();
    m_resolution.resize(4);

    Resize(m_window->GetWidth(), m_window->GetHeight());
  }

  void DxRenderer::Init() {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;

    // Disable face culling
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthBias = 0;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = TRUE;
    rasterDesc.ScissorEnable = FALSE;
    rasterDesc.MultisampleEnable = TRUE;
    rasterDesc.AntialiasedLineEnable = TRUE;

    HRESULT result = device->CreateRasterizerState(&rasterDesc, m_rasterizerState.GetAddressOf());
    assert(SUCCEEDED(result));

    dc->RSSetState(m_rasterizerState.Get());

    // Create constant buffer
    result = m_constantBuffer.Init();
    assert(SUCCEEDED(result));

    // Create samplers
    {
      D3D11_SAMPLER_DESC desc {};
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
      desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.MaxLOD = D3D11_FLOAT32_MAX;
      desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
      result = DxContext::Get()->d3d11Device->CreateSamplerState(&desc, m_pointSampler.GetAddressOf());
      assert(SUCCEEDED(result));
    }
    {
      D3D11_SAMPLER_DESC desc {};
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.MaxLOD = D3D11_FLOAT32_MAX;
      desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
      result = DxContext::Get()->d3d11Device->CreateSamplerState(&desc, m_linearSampler.GetAddressOf());
      assert(SUCCEEDED(result));
    }
    {
      D3D11_SAMPLER_DESC desc {};
      desc.Filter = D3D11_FILTER_ANISOTROPIC;
      desc.MaxAnisotropy = 8;
      desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      desc.MaxLOD = D3D11_FLOAT32_MAX;
      desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
      result = DxContext::Get()->d3d11Device->CreateSamplerState(&desc, m_anisotropicSampler.GetAddressOf());
      assert(SUCCEEDED(result));
    }

    // Set Samplers
    dc->PSSetSamplers(0, 1, m_pointSampler.GetAddressOf());
    dc->PSSetSamplers(1, 1, m_linearSampler.GetAddressOf());
    dc->PSSetSamplers(2, 1, m_anisotropicSampler.GetAddressOf());

    // Init skybox shaders
    m_skyboxPipeline.Init(kSkyShaderPath, ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    {
      auto texture = TextureManager::Get()->GetTexture(kSkyboxPath);
      m_skyTextureView = texture->GetResourceView();
      m_skyTexture = texture->GetResource();
    }

    // Create IBL shader pipelines
    diffuseIblPipeline.Init(L"Assets/Shaders/iblDiffuse.hlsl", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

    // Generate IBL textures

    // Generate TextureCube
    uint32_t textureSize = 8;
    D3D11_TEXTURE2D_DESC diffuseDesc {
      textureSize,
      textureSize,
      1,
      6,
      DXGI_FORMAT_R16G16B16A16_FLOAT,
      { 1, 0 },
      D3D11_USAGE_DEFAULT,
      D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
      0,
      D3D11_RESOURCE_MISC_TEXTURECUBE
    };
    result = device->CreateTexture2D(
      &diffuseDesc,
      nullptr,
      diffuseReflectionTexture.GetAddressOf()
    );
    assert(SUCCEEDED(result));

    // Create SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
    srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube = { 0, 1 };

    result = device->CreateShaderResourceView(
      diffuseReflectionTexture.Get(),
      nullptr,
      diffuseReflectionView.GetAddressOf()
    );
    assert(SUCCEEDED(result));

    // Create RTVs
    ComPtr<ID3D11RenderTargetView> diffuseRTVs[6];
    for (uint32_t i = 0; i < 6; ++i) {
      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc {};
      rtvDesc.Format = diffuseDesc.Format;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
      rtvDesc.Texture2DArray.MipSlice = 0;
      rtvDesc.Texture2DArray.ArraySize = 1;
      rtvDesc.Texture2DArray.FirstArraySlice = i;

      result = device->CreateRenderTargetView(
        diffuseReflectionTexture.Get(),
        &rtvDesc,
        diffuseRTVs[i].GetAddressOf()
      );
      assert(SUCCEEDED(result));
    }

    // Create buffer
    diffuseBuffer.Init();

    // Generate input data
    glm::vec4 front[6] = {
      { 1, 0, 0, 0 },
      { -1, 0, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, -1, 0, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, -1, 0 },
    };

    glm::vec4 right[6] = {
      { 0, 0, -1, 0 },
      { 0, 0, 1, 0 },
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { 1, 0, 0, 0 },
      { -1, 0, 0, 0 },
    };

    glm::vec4 up[6] = {
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, -1, 0 },
      { 0, 0, 1, 0 },
      { 0, 1, 0, 0 },
      { 0, 1, 0, 0 },
    };

    glm::mat4 matrices[6];
    for (uint32_t i = 0; i < 6; ++i) {
      // It's inversed since glm matrices are column major
      matrices[i] = glm::transpose(glm::mat4(
        right[i], up[i], front[i], { 0, 0, 0, 1 }
      ));
    }

    // Render onto faces
    D3D11_VIEWPORT viewport {
      0.0f,
      0.0f,
      float(textureSize),
      float(textureSize),
      0.0f,
      1.0f,
    };
    UpdateConstantBuffer(0.0f);

    ComPtr<ID3D11Texture2D> texture;
    result = m_skyTexture->QueryInterface(IID_PPV_ARGS(texture.GetAddressOf()));
    assert(SUCCEEDED(result));
    D3D11_TEXTURE2D_DESC desc = {};
    texture->GetDesc(&desc);
    diffuseBuffer.data.cubemapSize = float(desc.Width);

    dc->RSSetViewports(1, &viewport);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->VSSetConstantBuffers(13, 1, diffuseBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(13, 1, diffuseBuffer.GetAddressOf());
    dc->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    dc->PSSetShaderResources(0, 1, &m_skyTextureView);
    diffuseIblPipeline.Bind();
    for (uint32_t i = 0; i < 6; ++i) {
      dc->OMSetRenderTargets(1, diffuseRTVs[i].GetAddressOf(), nullptr);
      diffuseBuffer.data.normal = front[i];
      diffuseBuffer.data.viewMatInv = matrices[i];
      diffuseBuffer.ApplyChanges();
      dc->Draw(3, 0);
    }

    TextureManager::SaveToDDS(
      Engine::GetDirectory(L"cubemap.dds"),
      diffuseReflectionTexture.Get(),
      TextureManager::FileFormat(DXGI_FORMAT_R16G16B16A16_FLOAT),//::BC6_UNSIGNED,
      false
    );
  }

  void DxRenderer::Cleanup() {
    m_pointSampler.Reset();
    m_linearSampler.Reset();
    m_anisotropicSampler.Reset();
    m_constantBuffer.Reset();
  }

  void DxRenderer::Update(float deltaTime) {
  }

  void DxRenderer::Render(float time, float deltaTime) {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    auto targetView = m_window->GetTargetView();
    auto targetViewHdr = m_window->GetTargetViewHdr();
    auto targetSrvHdr = m_window->GetTargetSrvHdr();
    auto depthStencilView = m_window->GetDepthStencilView();
    auto depthStencilState = m_window->GetDepthStencilState();

    // Constant buffer magic
    UpdateConstantBuffer(time);

    // Set constant buffers
    LightSystem::Get()->CommitChanges();
    ID3D11Buffer* buffers[] {
      m_constantBuffer.Get(),
      LightSystem::Get()->GetConstantBuffer()
    };

    dc->VSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
    dc->PSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
    dc->GSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
    dc->HSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
    dc->DSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);

    // Set target
    D3D11_VIEWPORT viewport = m_window->GetViewport();
    DxContext::Get()->d3d11DeviceContext->RSSetViewports(1, &viewport);

    dc->OMSetRenderTargets(1, targetViewHdr.GetAddressOf(), depthStencilView.Get());
    dc->OMSetDepthStencilState(depthStencilState.Get(), 0);

    float clearColor[4] = { 0.12f, 0.12f, 0.12f, 1.0f };
    dc->ClearRenderTargetView(targetViewHdr.Get(), clearColor);
    dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

    MeshSystem::Get()->Render(deltaTime);
    RenderSkybox();

    // Resolve HDR -> LDR
    PostProcess::Get()->Resolve(targetSrvHdr.Get(), targetView.Get());
  }

  void DxRenderer::Resize(uint32_t width, uint32_t height) {
    m_camera->Resize(width, height);

    m_resolution[0] = static_cast<float>(width);
    m_resolution[1] = static_cast<float>(height);
    m_resolution[2] = 1.0f / m_resolution[0];
    m_resolution[3] = 1.0f / m_resolution[1];
  }

  void DxRenderer::SetNormalVisMode(bool isNormalVisMode) {
    m_isNormalVisMode = isNormalVisMode;
  }

  bool DxRenderer::GetNormalVisMode() const {
    return m_isNormalVisMode;
  }

  float DxRenderer::GetEvFactor() const {
    return m_evFactor;
  }

  void DxRenderer::SetEvFactor(float evFactor) {
    m_evFactor = evFactor;
  }

  void DxRenderer::RenderSkybox() {
    auto dc = DxContext::Get()->d3d11DeviceContext.Get();
    m_skyboxPipeline.Bind();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //dc->PSSetShaderResources(0, 1, &m_skyTextureView);
    dc->PSSetShaderResources(0, 1, diffuseReflectionView.GetAddressOf());
    dc->Draw(3, 0);
  }

  void DxRenderer::UpdateConstantBuffer(float time) {
    glm::mat4 view = m_camera->GetViewMatrix();
    glm::mat4 projection = m_camera->GetProjectionMatrix();

    m_constantBuffer.data.viewMatrix = view;
    m_constantBuffer.data.projectionMatrix = projection;

    glm::vec3 toTl = glm::vec3(m_camera->ClipToWorld(glm::vec4(-1.0f, 3.0f, 0.0f, 1.0f))) - m_camera->GetPosition();
    glm::vec3 toBl = glm::vec3(m_camera->ClipToWorld(glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f))) - m_camera->GetPosition();
    glm::vec3 toBr = glm::vec3(m_camera->ClipToWorld(glm::vec4(3.0f, -1.0f, 0.0f, 1.0f))) - m_camera->GetPosition();
    m_constantBuffer.data.frustumTL[0] = toTl.x;
    m_constantBuffer.data.frustumTL[1] = toTl.y;
    m_constantBuffer.data.frustumTL[2] = toTl.z;
    m_constantBuffer.data.frustumTL[3] = 1;

    m_constantBuffer.data.frustumBL[0] = toBl.x;
    m_constantBuffer.data.frustumBL[1] = toBl.y;
    m_constantBuffer.data.frustumBL[2] = toBl.z;
    m_constantBuffer.data.frustumBL[3] = 1;

    m_constantBuffer.data.frustumBR[0] = toBr.x;
    m_constantBuffer.data.frustumBR[1] = toBr.y;
    m_constantBuffer.data.frustumBR[2] = toBr.z;
    m_constantBuffer.data.frustumBR[3] = 1;

    const glm::vec3& cameraPos = m_camera->GetPosition();
    m_constantBuffer.data.cameraPosition[0] = cameraPos.x;
    m_constantBuffer.data.cameraPosition[1] = cameraPos.y;
    m_constantBuffer.data.cameraPosition[2] = cameraPos.z;
    m_constantBuffer.data.cameraPosition[3] = 1;
    std::memcpy(m_constantBuffer.data.resolution, m_resolution.data(), m_resolution.size() * sizeof(float));

    m_constantBuffer.data.time = time;
    m_constantBuffer.data.isNormalVisMode = m_isNormalVisMode;
    m_constantBuffer.data.evFactor = m_evFactor;

    m_constantBuffer.ApplyChanges();
  }
}
