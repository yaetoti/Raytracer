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

#include "Flame/utils/draggers/IDragger.h"

#include "Flame/utils/Random.h"
#include "Flame/math/MathUtils.h"
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

    HRESULT result = DxContext::Get()->d3d11Device->CreateRasterizerState(&rasterDesc, m_rasterizerState.GetAddressOf());
    assert(SUCCEEDED(result));

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

    // Create resolve shaders TODO move
    m_resolveVertexShader.Init(L"Assets/Shaders/resolve.hlsl", nullptr, 0);
    m_resolvePixelShader.Init(L"Assets/Shaders/resolve.hlsl");
  }

  void DxRenderer::Cleanup() {
    m_resolveVertexShader.Reset();
    m_resolvePixelShader.Reset();

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

    // Set target
    //dc->OMSetRenderTargets(1, targetView.GetAddressOf(), depthStencilView.Get());
    dc->OMSetRenderTargets(1, targetViewHdr.GetAddressOf(), depthStencilView.Get());
    dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
    dc->RSSetState(m_rasterizerState.Get());

    float clearColor[4] = { 0.12f, 0.12f, 0.12f, 1.0f };
    dc->ClearRenderTargetView(targetViewHdr.Get(), clearColor);
    dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

    // Set Samplers
    dc->PSSetSamplers(0, 1, m_pointSampler.GetAddressOf());
    dc->PSSetSamplers(1, 1, m_linearSampler.GetAddressOf());
    dc->PSSetSamplers(2, 1, m_anisotropicSampler.GetAddressOf());

    // Constant buffer magic
    {
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

      m_constantBuffer.ApplyChanges();
    }

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

    MeshSystem::Get()->Render(deltaTime);

    // Resolve HDR -> LDR
    dc->OMSetRenderTargets(1, targetView.GetAddressOf(), depthStencilView.Get());
    dc->ClearRenderTargetView(targetView.Get(), clearColor);
    dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

    dc->VSSetShader(m_resolveVertexShader.GetShader(), nullptr, 0);
    dc->PSSetShader(m_resolvePixelShader.GetShader(), nullptr, 0);
    dc->IASetInputLayout(nullptr);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->PSSetShaderResources(0, 1, targetSrvHdr.GetAddressOf());
    dc->Draw(3, 0);
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
}
