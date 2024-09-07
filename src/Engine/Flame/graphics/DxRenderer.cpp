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

    // TODO HW10
    // For DirectLights and SpotLights
    MeshSystem::Get()->RenderDepth2D();
    // For PointLights
    MeshSystem::Get()->RenderDepthCubemaps({});

    // Update ConstantBuffers
    UpdateFrameBuffer(time);
    // TODO pass position
    UpdateViewBuffer();

    // Set ConstantBuffers
    LightSystem::Get()->CommitChanges();
    ID3D11Buffer* buffers[3];
    buffers[kFrameCBufferId] = m_frameCBuffer.Get();
    buffers[kViewCBufferId] = m_viewCBuffer.Get();
    buffers[kLightCBufferId] = LightSystem::Get()->GetConstantBuffer();

    DxContext::Get()->SetPipelineConstantBuffers(0, buffers);

    // Set target
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
