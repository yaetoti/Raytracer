#include "DxRenderer.h"
#include "Flame/engine/MeshSystem.h"
#include "Flame/engine/OpaqueGroup.h"
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
  DxRenderer::DxRenderer(Window* window)
  : m_window(window) {
    m_input = &m_window->GetInputSystem();
    m_resolution.resize(4);

    m_camera = std::make_shared<AlignedCamera>(m_window->GetWidth(), m_window->GetHeight(), 90.0f, 0.01f, 1000.0f);
    m_camera->SetPosition(glm::vec3(0, 0, 2));

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

  void DxRenderer::UpdateCamera(float deltaTime) {
    static float baseSpeed = 0.5f;
    float speed = baseSpeed;
    float rollSpeedDeg = 90.0f;

    // Movement speed
    if (m_input->IsKeyPressed(VK_SHIFT)) {
      speed *= 5;
    }
    if (m_input->GetScrollDelta() != 0.0f) {
      baseSpeed += baseSpeed * 0.05f * m_input->GetScrollDelta();
    }

    // Movement
    if (m_input->IsKeyPressed('A')) {
      m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetRightUnit() * -speed * deltaTime);
    }
    if (m_input->IsKeyPressed('D')) {
      m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetRightUnit() * speed * deltaTime);
    }
    if (m_input->IsKeyPressed('W')) {
      m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetFrontUnit() * speed * deltaTime);
    }
    if (m_input->IsKeyPressed('S')) {
      m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetFrontUnit() * -speed * deltaTime);
    }
    if (m_input->IsKeyPressed('Q')) {
      m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * -speed * deltaTime);
    }
    if (m_input->IsKeyPressed('E')) {
      m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * speed * deltaTime);
    }

    // Rotation
    if (m_input->IsMouseButtonPressed(MouseButton::LEFT)) {
      static float rotationSpeedDeg = -180.0f;
      constexpr float sensitivity = 1.0f;
      auto[x, y] = m_input->GetCursorPos();
      float width = m_window->GetWidth();
      float height = m_window->GetHeight();
      float halfWidth = width * 0.5f;
      float halfHeight = height * 0.5f;

#if 1
      float deltaX = (x - halfWidth) / width * sensitivity;
      float deltaY = (y - halfHeight) / height * sensitivity;
      m_camera->Rotate(deltaY * rotationSpeedDeg * deltaTime, deltaX * rotationSpeedDeg * deltaTime);
#else
      auto[lastX, lastY] = m_input->GetLastCursorPos();
      float deltaX = ((x - lastX) / width) * sensitivity;
      float deltaY = ((y - lastY) / height) * sensitivity;
      m_camera->Rotate(deltaY * rotationSpeedDeg, deltaX * rotationSpeedDeg);
#endif
    }

    // Dragging
    static std::unique_ptr<IDragger> dragger = nullptr;

    if (m_input->IsMouseButtonPressed(MouseButton::RIGHT)) {
      auto[x, y] = m_input->GetCursorPos();
      Ray ray = m_camera->GetRay(static_cast<uint32_t>(x), static_cast<uint32_t>(y));

      if (!dragger) {
        HitRecord<MeshSystem::HitResult> record;
        if (MeshSystem::Get()->Hit(ray, record, 0.0f, 1000.0f)) {
          dragger = DraggerFactory::CreateDragger(record, m_camera->GetPosition(), m_camera->GetFrontUnit());
        }
      } else {
        dragger->Drag(ray, m_camera->GetFrontUnit());
      }
    } else {
      if (dragger != nullptr) {
        dragger = nullptr;
      }
    }
  }

  void DxRenderer::Update(float deltaTime) {
    UpdateCamera(deltaTime);
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
