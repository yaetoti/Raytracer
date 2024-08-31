#pragma once
#include <d3d11.h>
#include <string>
#include <wrl/client.h>

namespace Flame {
  struct VertexShader final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ID3DBlob* GetBlob() const;
    ID3D11VertexShader* GetShader() const;
    ID3D11InputLayout* GetInputLayout() const;

    void Reset();
    void Init(const std::wstring& path, D3D11_INPUT_ELEMENT_DESC* inputDesc, uint32_t descSize);

  private:
    ComPtr<ID3DBlob> m_blob;
    ComPtr<ID3D11VertexShader> m_shader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
  };
}
