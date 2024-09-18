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
#include <imgui.h>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <winnt.h>
#include <Flame/engine/Engine.h>
#include <Flame/engine/ReflectionCapture.h>
#include <Flame/engine/TextureManager.h>

#include "Flame/utils/draggers/IDragger.h"

#include "Flame/utils/Random.h"
#include "Flame/math/MathUtils.h"
#include "PostProcess.h"
#include "glm/ext.hpp"

#include "buffers/CBufferIndices.h"

namespace Flame {
  DxRenderer::DxRenderer(std::shared_ptr<Window> window, std::shared_ptr<AlignedCamera> camera)
  : m_window(std::move(window))
  , m_camera(std::move(camera)) {
    m_input = &m_window->GetInputSystem();

    Resize(m_window->GetWidth(), m_window->GetHeight());
  }

  DxRenderer::~DxRenderer() {
    Cleanup();
  }

  void DxRenderer::Init() {
    auto device = DxContext::Get()->d3d11Device;
    auto dc = DxContext::Get()->d3d11DeviceContext;

    // RasterizerState
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

    // Set RasterizerState
    dc->RSSetState(m_rasterizerState.Get());

    // ConstantBuffers
    result = m_frameCBuffer.Init();
    assert(SUCCEEDED(result));
    result = m_viewCBuffer.Init();
    assert(SUCCEEDED(result));

    // Samplers
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

    // Shaders
    m_skyboxPipeline.Init(kSkyShaderPath, ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    m_testPipeline.Init(L"Assets/Shaders/test.hlsl", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

    // IBL
    {
      auto texture = TextureManager::Get()->GetTexture(kSkyboxPath);
      m_skyTextureView = texture->GetResourceView();
      m_skyTexture = texture->GetResource();
    }
  }

  void DxRenderer::Cleanup() {
    m_directLightsCount = 0;
    m_spotLightsCount = 0;

    m_pointSampler.Reset();
    m_linearSampler.Reset();
    m_anisotropicSampler.Reset();
    m_frameCBuffer.Reset();
    m_viewCBuffer.Reset();
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

    // ImGUI settings
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Enable diffuse", &m_diffuseEnabled);
    ImGui::Checkbox("Enable specular", &m_specularEnabled);
    ImGui::Checkbox("Enable IBL diffuse", &m_iblDiffuseEnabled);
    ImGui::Checkbox("Enable IBL specular", &m_iblSpecularEnabled);
    ImGui::Checkbox("Override roughness", &m_overwriteRoughness);
    ImGui::SliderFloat("Roughness", &m_roughness, 0.0f, 1.0f);
    ImGui::End();

    {
      static bool firstRun = true;
      if (!firstRun) {
        return;
      }

      //firstRun = false;
    }

    // Set common CBuffers
    ID3D11Buffer* buffers[3];
    buffers[kFrameCBufferId] = m_frameCBuffer.Get();
    buffers[kViewCBufferId] = m_viewCBuffer.Get();
    buffers[kLightCBufferId] = LightSystem::Get()->GetConstantBuffer();
    DxContext::Get()->SetPipelineConstantBuffers(0, buffers);

    // Update Frame CBuffer
    UpdateFrameBuffer(time);

    // Update light matrices
    UpdateMatricesDirect();
    UpdateMatricesSpot();
    LightSystem::Get()->CommitChanges();

    // Render ShadowMaps.
    InitShadowMapsDirect();
    InitShadowMapsSpot();
    InitShadowMapsPoint();

    RenderShadowMapsDirect();
    RenderShadowMapsSpot();
    RenderShadowMapsPoint();

    // Update Camera View
    UpdateViewBuffer();

    // Render scene
    D3D11_VIEWPORT viewport = m_window->GetViewport();
    dc->RSSetViewports(1, &viewport);

    dc->OMSetRenderTargets(1, targetViewHdr.GetAddressOf(), depthStencilView.Get());
    dc->OMSetDepthStencilState(depthStencilState.Get(), 0);

    float clearColor[4] = { 0.12f, 0.12f, 0.12f, 1.0f };
    dc->ClearRenderTargetView(targetViewHdr.Get(), clearColor);
    dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

    // Draw something in the foreground
    // m_testPipeline.Bind();
    // dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // dc->Draw(3, 0);

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

  bool DxRenderer::DiffuseEnabled() const {
    return m_diffuseEnabled;
  }

  void DxRenderer::SetDiffuseEnabled(bool diffuseEnabled) {
    m_diffuseEnabled = diffuseEnabled;
  }

  bool DxRenderer::SpecularEnabled() const {
    return m_specularEnabled;
  }

  void DxRenderer::SetSpecularEnabled(bool specularEnabled) {
    m_specularEnabled = specularEnabled;
  }

  bool DxRenderer::IblDiffuseEnabled() const {
    return m_iblDiffuseEnabled;
  }

  void DxRenderer::SetIblDiffuseEnabled(bool iblDiffuseEnabled) {
    m_iblDiffuseEnabled = iblDiffuseEnabled;
  }

  bool DxRenderer::IblSpecularEnabled() const {
    return m_iblSpecularEnabled;
  }

  void DxRenderer::SetIblSpecularEnabled(bool iblSpecularEnabled) {
    m_iblSpecularEnabled = iblSpecularEnabled;
  }

  bool DxRenderer::OverwriteRoughness() const {
    return m_overwriteRoughness;
  }

  void DxRenderer::SetOverwriteRoughness(bool overwriteRoughness) {
    m_overwriteRoughness = overwriteRoughness;
  }

  float DxRenderer::Roughness() const {
    return m_roughness;
  }

  void DxRenderer::SetRoughness(float roughness) {
    m_roughness = roughness;
  }

  void DxRenderer::InitShadowMapsDirect() {
    HRESULT result;
    uint32_t directLightsCount = LightSystem::Get()->GetDirectLights().size();

    // DirectLights
    if (m_directLightsCount != directLightsCount) {
      m_directLightsCount = directLightsCount;

      // TextureArray
      D3D11_TEXTURE2D_DESC desc {
        kShadowMapResolution,
        kShadowMapResolution,
        1,
        m_directLightsCount,
        DXGI_FORMAT_R24G8_TYPELESS,
        { 1, 0 },
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
        0,
        0
      };
      result = DxContext::Get()->d3d11Device->CreateTexture2D(&desc, nullptr, m_shadowMapArrayDirect.ReleaseAndGetAddressOf());
      assert(SUCCEEDED(result));

      // DSV
      for (uint32_t i = 0; i < ARRAYSIZE(m_shadowMapDsvDirect); ++i) {
        D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc {};
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        depthDesc.Texture2DArray.FirstArraySlice = i;
        depthDesc.Texture2DArray.ArraySize = 1;
        depthDesc.Texture2DArray.MipSlice = 0;

        DxContext::Get()->d3d11Device->CreateDepthStencilView(m_shadowMapArrayDirect.Get(), &depthDesc, m_shadowMapDsvDirect[i].ReleaseAndGetAddressOf());
      }

      // SRV
      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc {};
      srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
      srvDesc.Texture2DArray.ArraySize = m_directLightsCount;
      srvDesc.Texture2DArray.FirstArraySlice = 0;
      srvDesc.Texture2DArray.MipLevels = 1;
      srvDesc.Texture2DArray.MostDetailedMip = 0;

      result = DxContext::Get()->d3d11Device->CreateShaderResourceView(m_shadowMapArrayDirect.Get(), &srvDesc, m_shadowMapSrvDirect.ReleaseAndGetAddressOf());
      assert(SUCCEEDED(result));
    }
  }

  void DxRenderer::UpdateMatricesDirect() {
    // Get frustum corners and center in WS
    auto frustumCornersWS = m_camera->GetFrustumCornersWS();
    glm::vec3 center(0.0f);
    for (uint32_t i = 0; i < 8; ++i) {
      center += glm::vec3(frustumCornersWS[i]);
    }
    center /= 8;

    float texelSize = 1.0f / kShadowMapResolution;
    float xyHalfSide = glm::length(glm::vec3(frustumCornersWS[7]) - m_camera->GetPosition());
    float texelSizeWorld = 2 * xyHalfSide * texelSize;
    // Moving in discrete stemp
    center = glm::mod(m_camera->GetPosition(), glm::vec3(texelSizeWorld)) * texelSizeWorld;

    // Preprocess lights
    for (uint32_t lightId = 0; lightId < LightSystem::Get()->GetDirectLights().size(); ++lightId) {
      std::shared_ptr<DirectLight>& light = LightSystem::Get()->GetDirectLights().at(lightId);

      // We calculate projection from center, so we need to include front and back
      glm::vec3 lightDir = light->direction;
      glm::mat4 lightView = MathUtils::ViewFromDir(lightDir, center);

      // Calculate frustum AABB in light's VS
      float zMin = std::numeric_limits<float>::infinity();
      float zMax = -std::numeric_limits<float>::infinity();
      for (uint32_t i = 0; i < 8; ++i) {
        glm::vec3 positionVS = glm::vec3(lightView * frustumCornersWS[i]);
        zMin = glm::min(zMin, positionVS.z);
        zMax = glm::max(zMax, positionVS.z);
      }

      // XY = frustum radius

      float zHalfSide = 0.5f * std::abs(zMax - zMin);

      // glm::mat4 lightProjection = MathUtils::Orthographic(
      //   xyHalfSide,
      //   -xyHalfSide,
      //   xyHalfSide,
      //   -xyHalfSide,
      //   zHalfSide,
      //   -(zHalfSide + kDirectShadowPadding)
      // );
      glm::mat4 lightProjection = MathUtils::Orthographic(
        xyHalfSide,
        -xyHalfSide,
        xyHalfSide,
        -xyHalfSide,
        xyHalfSide,
        -xyHalfSide
      );

      // Update data
      light->viewMat = lightView;
      light->projectionMat = lightProjection;
    }
  }

  void DxRenderer::RenderShadowMapsDirect() {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();

    for (uint32_t i = 0; i < LightSystem::Get()->GetDirectLights().size(); ++i) {
      std::shared_ptr<DirectLight>& light = LightSystem::Get()->GetDirectLights().at(i);

      // Update ViewCBuffer
      m_viewCBuffer.data.viewMatrix = light->viewMat;
      m_viewCBuffer.data.projectionMatrix = light->projectionMat;
      m_viewCBuffer.ApplyChanges();

      // Render ShadowMap
      D3D11_VIEWPORT viewport {};
      viewport.Width = kShadowMapResolution;
      viewport.Height = kShadowMapResolution;
      viewport.TopLeftX = 0.0f;
      viewport.TopLeftY = 0.0f;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;

      dc->RSSetViewports(1, &viewport);
      dc->OMSetRenderTargets(1, PtrProxy<ID3D11RenderTargetView*>(nullptr).Ptr(), m_shadowMapDsvDirect[i].Get());
      dc->ClearDepthStencilView(m_shadowMapDsvDirect[i].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

      MeshSystem::Get()->RenderDepth2D();
    }
  }

  void DxRenderer::InitShadowMapsSpot() {
    HRESULT result;
    uint32_t spotLightsCount = LightSystem::Get()->GetSpotLights().size();

    if (m_spotLightsCount != spotLightsCount) {
      m_spotLightsCount = spotLightsCount;

      // TextureArray
      D3D11_TEXTURE2D_DESC desc {
        kShadowMapResolution,
        kShadowMapResolution,
        1,
        m_spotLightsCount,
        DXGI_FORMAT_R24G8_TYPELESS,
        { 1, 0 },
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
        0,
        0
      };
      result = DxContext::Get()->d3d11Device->CreateTexture2D(&desc, nullptr, m_shadowMapArraySpot.ReleaseAndGetAddressOf());
      assert(SUCCEEDED(result));

      // DSV
      for (uint32_t i = 0; i < ARRAYSIZE(m_shadowMapDsvSpot); ++i) {
        D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc {};
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        depthDesc.Texture2DArray.FirstArraySlice = i;
        depthDesc.Texture2DArray.ArraySize = 1;
        depthDesc.Texture2DArray.MipSlice = 0;

        DxContext::Get()->d3d11Device->CreateDepthStencilView(m_shadowMapArraySpot.Get(), &depthDesc, m_shadowMapDsvSpot[i].ReleaseAndGetAddressOf());
      }

      // SRV
      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc {};
      srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
      srvDesc.Texture2DArray.ArraySize = m_spotLightsCount;
      srvDesc.Texture2DArray.FirstArraySlice = 0;
      srvDesc.Texture2DArray.MipLevels = 1;
      srvDesc.Texture2DArray.MostDetailedMip = 0;

      result = DxContext::Get()->d3d11Device->CreateShaderResourceView(m_shadowMapArraySpot.Get(), &srvDesc, m_shadowMapSrvSpot.ReleaseAndGetAddressOf());
      assert(SUCCEEDED(result));
    }
  }

  void DxRenderer::UpdateMatricesSpot() {
    // Matrices: no need for view, we set it in application before calling render View
    // Perspective: need fov, aspect == 1, near == 0.01, far = frustum's maxZ

    auto frustumCornersWS = m_camera->GetFrustumCornersWS();

    // Preprocess lights
    for (uint32_t lightId = 0; lightId < LightSystem::Get()->GetSpotLights().size(); ++lightId) {
      auto& light = LightSystem::Get()->GetSpotLights().at(lightId);

      // Calculate frustum AABB in light's VS
      float zMax = -std::numeric_limits<float>::infinity();
      glm::mat4 viewMat = light->GetViewMatrix();
      for (uint32_t i = 0; i < 8; ++i) {
        glm::vec3 positionVS = glm::vec3(viewMat * frustumCornersWS[i]);
        zMax = glm::max(zMax, positionVS.z);
      }

      constexpr float zMin = 0.01f;
      float zHalfSide = 0.5f * std::abs(zMax - zMin);

      glm::mat4 lightProjection = MathUtils::Perspective(
        2 * glm::acos(light->cutoffCosineOuter),
        1.0f,
        zMin,
        std::max(zMax, zMin + 100.0f)
      );

      // Update data
      light->projectionMat = lightProjection;
    }
  }

  void DxRenderer::RenderShadowMapsSpot() {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();

    for (uint32_t i = 0; i < LightSystem::Get()->GetSpotLights().size(); ++i) {
      auto& light = LightSystem::Get()->GetSpotLights().at(i);

      // Update ViewCBuffer
      m_viewCBuffer.data.viewMatrix = light->GetViewMatrix();
      m_viewCBuffer.data.projectionMatrix = light->projectionMat;
      m_viewCBuffer.ApplyChanges();

      // Render ShadowMap
      D3D11_VIEWPORT viewport {};
      viewport.Width = kShadowMapResolution;
      viewport.Height = kShadowMapResolution;
      viewport.TopLeftX = 0.0f;
      viewport.TopLeftY = 0.0f;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;

      dc->RSSetViewports(1, &viewport);
      dc->OMSetRenderTargets(1, PtrProxy<ID3D11RenderTargetView*>(nullptr).Ptr(), m_shadowMapDsvSpot[i].Get());
      dc->ClearDepthStencilView(m_shadowMapDsvSpot[i].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

      MeshSystem::Get()->RenderDepth2D();
    }
  }

  void DxRenderer::InitShadowMapsPoint() {
    auto device = DxContext::Get()->d3d11Device.Get();
    HRESULT result;

    uint32_t pointLightsCount = LightSystem::Get()->GetPointLights().size();
    if (m_pointLightsCount == pointLightsCount) {
      return;
    }

    m_pointLightsCount = pointLightsCount;

    // Texture
    D3D11_TEXTURE2D_DESC textureDesc {};
    textureDesc.Width = kShadowMapResolution;
    textureDesc.Height = kShadowMapResolution;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = pointLightsCount * 6;
    textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    textureDesc.SampleDesc = { 1, 0 };
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    result = device->CreateTexture2D(&textureDesc, nullptr, m_shadowMapArrayPoint.ReleaseAndGetAddressOf());
    assert(SUCCEEDED(result));

    // DSV
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
    dsvDesc.Texture2DArray.MipSlice = 0;
    dsvDesc.Texture2DArray.FirstArraySlice = 0;
    result = device->CreateDepthStencilView(m_shadowMapArrayPoint.Get(), &dsvDesc, m_shadowMapDsvPoint.ReleaseAndGetAddressOf());
    assert(SUCCEEDED(result));

    // SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDecs = {};
    srvDecs.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDecs.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
    srvDecs.TextureCubeArray.MipLevels = 1;
    srvDecs.TextureCubeArray.MostDetailedMip = 0;
    srvDecs.TextureCubeArray.First2DArrayFace = 0;
    srvDecs.TextureCubeArray.NumCubes = m_pointLightsCount;
    result = device->CreateShaderResourceView(m_shadowMapArrayPoint.Get(), &srvDecs, m_shadowMapSrvPoint.ReleaseAndGetAddressOf());
    assert(SUCCEEDED(result));
  }

  void DxRenderer::RenderShadowMapsPoint() {
    auto dc = DxContext::Get()->d3d11DeviceContext.Get();

    std::vector<glm::vec3> positions(m_pointLightsCount);
    for (uint32_t i = 0; i < m_pointLightsCount; ++i) {
      positions[i] = LightSystem::Get()->GetPointLights()[i]->GetPositionWS();
    }

    // TODO Ideally it should be equal to light illuminance range, but currently there is only one matrix per all lights
    float far = 1000.0f;
    m_viewCBuffer.data.projectionMatrix = MathUtils::Perspective(glm::pi<float>() * 0.5f, 1.0f, 0.01f, far);
    m_viewCBuffer.ApplyChanges();

    D3D11_VIEWPORT viewport;
    viewport.Width = kShadowMapResolution;
    viewport.Height = kShadowMapResolution;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    dc->RSSetViewports(1, &viewport);
    dc->OMSetRenderTargets(1, PtrProxy<ID3D11RenderTargetView*>(nullptr).Ptr(), m_shadowMapDsvPoint.Get());
    dc->ClearDepthStencilView(m_shadowMapDsvPoint.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

    MeshSystem::Get()->RenderDepthCubemaps(positions);
  }

  void DxRenderer::RenderSkybox() {
    auto dc = DxContext::Get()->d3d11DeviceContext.Get();
    m_skyboxPipeline.Bind();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->PSSetShaderResources(0, 1, &m_skyTextureView);
    //dc->PSSetShaderResources(0, 1, m_diffuseTexture->GetResourceViewAddress());
    //dc->PSSetShaderResources(0, 1, m_specularTexture->GetResourceViewAddress());
    dc->Draw(3, 0);
  }

  void DxRenderer::UpdateFrameBuffer(float time) {
    m_frameCBuffer.data.time = time;
    *m_frameCBuffer.data.isNormalVisMode = m_isNormalVisMode;

    *m_frameCBuffer.data.diffuseEnabled = m_diffuseEnabled;
    *m_frameCBuffer.data.specularEnabled = m_specularEnabled;
    *m_frameCBuffer.data.iblDiffuseEnabled = m_iblDiffuseEnabled;
    *m_frameCBuffer.data.iblSpecularEnabled = m_iblSpecularEnabled;
    *m_frameCBuffer.data.overwriteRoughness = m_overwriteRoughness;
    m_frameCBuffer.data.roughness = m_roughness;

    m_frameCBuffer.ApplyChanges();
  }

  void DxRenderer::UpdateViewBuffer() {
    m_viewCBuffer.data.viewMatrix = m_camera->GetViewMatrix();
    m_viewCBuffer.data.projectionMatrix = m_camera->GetProjectionMatrix();

    glm::vec3 toTl = glm::vec3(m_camera->ClipToWorld(glm::vec4(-1.0f, 3.0f, 0.0f, 1.0f))) - m_camera->GetPosition();
    glm::vec3 toBl = glm::vec3(m_camera->ClipToWorld(glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f))) - m_camera->GetPosition();
    glm::vec3 toBr = glm::vec3(m_camera->ClipToWorld(glm::vec4(3.0f, -1.0f, 0.0f, 1.0f))) - m_camera->GetPosition();
    m_viewCBuffer.data.frustumTL = glm::vec4(toTl, 1.0);
    m_viewCBuffer.data.frustumBL = glm::vec4(toBl, 1.0);
    m_viewCBuffer.data.frustumBR = glm::vec4(toBr, 1.0);

    const glm::vec3& cameraPos = m_camera->GetPosition();
    m_viewCBuffer.data.cameraPosition = glm::vec4(cameraPos, 1.0);
    m_viewCBuffer.data.resolution = m_resolution;

    m_viewCBuffer.ApplyChanges();
  }

  ID3D11ShaderResourceView* DxRenderer::GetShadowMapSrvDirect() {
    return m_shadowMapSrvDirect.Get();
  }

  ID3D11ShaderResourceView* DxRenderer::GetShadowMapSrvSpot() {
    return m_shadowMapSrvSpot.Get();
  }

  ID3D11ShaderResourceView* DxRenderer::GetShadowMapSrvPoint() {
    return m_shadowMapSrvPoint.Get();
  }
}
