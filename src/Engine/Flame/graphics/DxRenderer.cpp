#include "DxRenderer.h"
#include "Flame/engine/MeshSystem.h"
#include "Flame/math/HitRecord.h"
#include "Flame/utils/PtrProxy.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <d3dcompiler.h>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#include "Flame/utils/draggers/IDragger.h"

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
  }

  void DxRenderer::Cleanup() {
    m_constantBuffer.Reset();
  }

  void DxRenderer::Update(float deltaTime) {
  }

  void DxRenderer::Render(float time, float deltaTime) {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    auto targetView = m_window->GetTargetView();
    auto depthStencilView = m_window->GetDepthStencilView();
    auto depthStencilState = m_window->GetDepthStencilState();

    // Set target
    dc->OMSetRenderTargets(1, targetView.GetAddressOf(), depthStencilView.Get());
    dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
    dc->RSSetState(m_rasterizerState.Get());

    float clearColor[4] = { 0.12f, 0.12f, 0.12f, 1.0f };
    dc->ClearRenderTargetView(targetView.Get(), clearColor);
    dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

    // Constant buffer magic
    {
      glm::mat4 view = m_camera->GetViewMatrix();
      glm::mat4 projection = m_camera->GetProjectionMatrix();

      m_constantBuffer.data.viewMatrix = view;
      m_constantBuffer.data.projectionMatrix = projection;

      m_constantBuffer.data.time = time;
      const glm::vec3& cameraPos = m_camera->GetPosition();
      m_constantBuffer.data.cameraPosition[0] = cameraPos.x;
      m_constantBuffer.data.cameraPosition[1] = cameraPos.y;
      m_constantBuffer.data.cameraPosition[2] = cameraPos.z;
      m_constantBuffer.data.cameraPosition[3] = 1;
      std::memcpy(m_constantBuffer.data.resolution, m_resolution.data(), m_resolution.size() * sizeof(float));
      m_constantBuffer.ApplyChanges();
    }

    dc->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    MeshSystem::Get()->Render(deltaTime);
  }

  void DxRenderer::Resize(uint32_t width, uint32_t height) {
    m_camera->Resize(width, height);

    m_resolution[0] = static_cast<float>(width);
    m_resolution[1] = static_cast<float>(height);
    m_resolution[2] = 1.0f / m_resolution[0];
    m_resolution[3] = 1.0f / m_resolution[1];
  }
}
