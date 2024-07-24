#include "DxRenderer.h"
#include <d3dcompiler.h>

namespace Flame {
  DxRenderer::DxRenderer(Window* window)
  : m_window(window) {
  }

  struct Vertex final {
    float position[4];
    float color[4];
  };

  void DxRenderer::Render() {
    ID3D11Device* device = DxContext::Get()->d3d11Device.Get();
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    ID3D11RenderTargetView* targetView = m_window->GetTargetView();
    HRESULT result;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11Buffer> vbo;

    // Compile shaders and create input layout
    {
      ComPtr<ID3DBlob> vertexBlob;
      ComPtr<ID3DBlob> pixelBlob;
      ComPtr<ID3DBlob> errorBlob;
      result = D3DCompileFromFile(
        L"Assets/Shaders/task3_1.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
        0,
        vertexBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
      );
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        printf("%ls\n", static_cast<LPCWSTR>(errorBlob->GetBufferPointer()));
        errorBlob.Reset();
      }

      result = D3DCompileFromFile(
        L"Assets/Shaders/task3_1.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
        0,
        pixelBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
      );
      assert(SUCCEEDED(result));
      if (FAILED(result)) {
        printf("%ls\n", static_cast<LPCWSTR>(errorBlob->GetBufferPointer()));
        errorBlob.Reset();
      }

      // TODO link shadersS
      result = device->CreateVertexShader(
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(),
        nullptr,
        vertexShader.GetAddressOf()
      );
      assert(SUCCEEDED(result));

      result = device->CreatePixelShader(
        pixelBlob->GetBufferPointer(),
        pixelBlob->GetBufferSize(),
        nullptr,
        pixelShader.GetAddressOf()
      );
      assert(SUCCEEDED(result));

      // Create input layout
      D3D11_INPUT_ELEMENT_DESC desc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      };
      result = device->CreateInputLayout(
        desc,
        ARRAYSIZE(desc),
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(),
        inputLayout.GetAddressOf()
      );
      assert(SUCCEEDED(result));
    }

    // Create buffers
    {
      Vertex vertices[3] = {
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

      result = device->CreateBuffer(&desc, &data, vbo.GetAddressOf());
      assert(SUCCEEDED(result));
    }


    // Clear
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    dc->ClearRenderTargetView(targetView, clearColor);

    // Draw triangle
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(inputLayout.Get());
    uint32_t offset = 0;
    uint32_t stride = 8 * sizeof(float);
    dc->IASetVertexBuffers(0, 1, vbo.GetAddressOf(), &stride, &offset);
    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    dc->PSSetShader(pixelShader.Get(), nullptr, 0);
    dc->Draw(3, 0);
  }

  void DxRenderer::Resize(uint32_t width, uint32_t height) {

  }
}
