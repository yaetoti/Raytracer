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

    // Update CBuffers
    UpdateFrameBuffer(time);
    LightSystem::Get()->CommitChanges();

    // Render ShadowMaps
    GenerateShadowMaps();
    RenderShadowMapsDirect();

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

  void DxRenderer::GenerateShadowMaps() {
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
        LightSystem::Get()->GetDirectLights().size(),
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
      D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc {};
      depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
      depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
      depthDesc.Texture2DArray.FirstArraySlice = 0;
      depthDesc.Texture2DArray.ArraySize = m_directLightsCount;
      depthDesc.Texture2DArray.MipSlice = 0;
      DxContext::Get()->d3d11Device->CreateDepthStencilView(m_shadowMapArrayDirect.Get(), &depthDesc, m_shadowMapDsvDirect.ReleaseAndGetAddressOf());

      // SRV
      // TODO SRV
    }
  }

  void DxRenderer::RenderShadowMapsDirect() {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();

    // Get frustum corners and center in WS
    auto frustumCornersWS = m_camera->GetFrustumCornersWS();
    glm::vec3 center(0.0f);
    for (uint32_t i = 0; i < 8; ++i) {
      center += glm::vec3(frustumCornersWS[i]);
    }
    center /= 8;

    // Preprocess lights
    for (std::shared_ptr<DirectLight>& light : LightSystem::Get()->GetDirectLights()) {
      glm::vec3 lightDir = light->direction;
      glm::mat4 viewMat = MathUtils::ViewFromDir(lightDir, center);

      // Calculate frustum AABB in light's VS
      glm::vec3 min(std::numeric_limits<float>::infinity());
      glm::vec3 max(-std::numeric_limits<float>::infinity());
      for (uint32_t i = 0; i < 8; ++i) {
        glm::vec3 positionVS = glm::vec3(viewMat * frustumCornersWS[i]);
        min = glm::min(min, positionVS);
        max = glm::max(max, positionVS);
      }

      // Get longest half-sides (Position at (0, 0, 0) in VS)
      // TODO replace with absolute longest side to avoid flickering on rotation.
      glm::vec3 halfSides = glm::max(glm::abs(min), glm::abs(max));
      float xySide = glm::max(halfSides.x, halfSides.y);
      // TODO add padding

      //glm::vec3 lightPos = center - halfSides.z * lightDir;
      //TransformSystem::Get()->At(0)->transform.SetPosition(center);
      glm::mat4 lightView = MathUtils::ViewFromDir(lightDir, center);
      glm::mat4 lightProjection = MathUtils::Orthographic(
        xySide,
        -xySide,
        xySide,
        -xySide,
        halfSides.z,
        -halfSides.z
      );

      // Update ViewCBuffer
      m_viewCBuffer.data.viewMatrix = lightView;
      m_viewCBuffer.data.projectionMatrix = lightProjection;
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
      dc->OMSetRenderTargets(1, PtrProxy<ID3D11RenderTargetView*>(nullptr).Ptr(), m_shadowMapDsvDirect.Get());
      dc->ClearDepthStencilView(m_shadowMapDsvDirect.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

      MeshSystem::Get()->RenderDepth2D();
    }
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
}
