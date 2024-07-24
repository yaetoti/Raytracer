#include "DxRenderer.h"
#include <d3dcompiler.h>
#include <iostream>

namespace Flame {
  DxRenderer::DxRenderer(Window* window)
  : m_window(window) {
    m_resolution.resize(4);
    Resize(m_window->GetWidth(), m_window->GetHeight());
  }

  void DxRenderer::Init() {
    LoadShaders();
    CreateBuffers();
  }

  void DxRenderer::Render(float time, float deltaTime) {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    ID3D11RenderTargetView* targetView = m_window->GetTargetView();

    float clearColor[4] = { 0.12f, 0.12f, 0.12f, 1.0f };
    dc->ClearRenderTargetView(targetView, clearColor);

    // Constant buffer magic
    PerFrame perFrame { .time = time };
    memcpy(perFrame.resolution, m_resolution.data(), m_resolution.size() * sizeof(float));
    dc->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &perFrame, 0, 0);

    // Draw triangle
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(m_inputLayout.Get());
    dc->IASetVertexBuffers(0, 1, m_vbo.GetAddressOf(), &kDataStride, &kDataOffset);
    dc->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    dc->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    dc->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    dc->Draw(3, 0);
  }

  void DxRenderer::Resize(uint32_t width, uint32_t height) {
    m_resolution[0] = static_cast<float>(width);
    m_resolution[1] = static_cast<float>(height);
    m_resolution[2] = 1.0f / m_resolution[0];
    m_resolution[3] = 1.0f / m_resolution[1];
  }

  bool DxRenderer::LoadShaders() {
    HRESULT result;
    ComPtr<ID3DBlob> errorBlob;

    // Load vertex
    result = D3DCompileFromFile(
      kShaderPath,
      nullptr,
      D3D_COMPILE_STANDARD_FILE_INCLUDE,
      "VSMain",
      "vs_5_0",
      D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
      0,
      m_vertexShaderBlob.GetAddressOf(),
      errorBlob.GetAddressOf()
    );
    assert(SUCCEEDED(result));
    if (FAILED(result)) {
      printf("%s\n", static_cast<LPCWSTR>(errorBlob->GetBufferPointer()));
      return false;
    }

    // Load pixel
    result = D3DCompileFromFile(
      kShaderPath,
      nullptr,
      D3D_COMPILE_STANDARD_FILE_INCLUDE,
      "PSMain",
      "ps_5_0",
      D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
      0,
      m_pixelShaderBlob.GetAddressOf(),
      errorBlob.GetAddressOf()
    );
    assert(SUCCEEDED(result));
    if (FAILED(result)) {
      printf("%s\n", static_cast<LPCWSTR>(errorBlob->GetBufferPointer()));
      return false;
    }

    // Create resources
    result = DxContext::Get()->d3d11Device->CreateVertexShader(
      m_vertexShaderBlob->GetBufferPointer(),
      m_vertexShaderBlob->GetBufferSize(),
      nullptr,
      m_vertexShader.GetAddressOf()
    );
    assert(SUCCEEDED(result));
    if (FAILED(result)) {
      return false;
    }

    result = DxContext::Get()->d3d11Device->CreatePixelShader(
      m_pixelShaderBlob->GetBufferPointer(),
      m_pixelShaderBlob->GetBufferSize(),
      nullptr,
      m_pixelShader.GetAddressOf()
    );
    assert(SUCCEEDED(result));
    if (FAILED(result)) {
      return false;
    }

    return true;
  }

  bool DxRenderer::CreateBuffers() {
    ID3D11Device* device = DxContext::Get()->d3d11Device.Get();
    HRESULT result;

    // Create input layout
    {
      D3D11_INPUT_ELEMENT_DESC desc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      };
      result = device->CreateInputLayout(
        desc,
        ARRAYSIZE(desc),
        m_vertexShaderBlob->GetBufferPointer(),
        m_vertexShaderBlob->GetBufferSize(),
        m_inputLayout.GetAddressOf()
      );
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        return false;
      }
    }

    // Create vbo
    {
      PerVertex vertices[3] = {
        {{ -0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{ 0.0f, 0.5f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
        {{ 0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }}
      };

      D3D11_BUFFER_DESC desc;
      desc.StructureByteStride = 0;
      desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      desc.ByteWidth = sizeof(vertices);
      desc.CPUAccessFlags = 0;
      desc.MiscFlags = 0;
      desc.Usage = D3D11_USAGE_DEFAULT;

      D3D11_SUBRESOURCE_DATA data;
      data.SysMemPitch = 0;
      data.SysMemSlicePitch = 0;
      data.pSysMem = vertices;

      result = device->CreateBuffer(&desc, &data, m_vbo.GetAddressOf());
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
       return false;
      }
    }

    // Create constant buffer
    {
      D3D11_BUFFER_DESC desc {
        32,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_CONSTANT_BUFFER,
        0,
        0,
        0
      };

      result = device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
       return false;
      }
    }

    return true;
  }
}
