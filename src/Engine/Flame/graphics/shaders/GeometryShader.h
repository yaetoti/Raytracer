#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

namespace Flame {
  struct GeometryShader final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ID3DBlob* GetBlob() const;
    ID3D11GeometryShader* GetShader() const;

    void Reset();
    void Init(const std::wstring& path);

  private:
    ComPtr<ID3DBlob> m_blob;
    ComPtr<ID3D11GeometryShader> m_shader;
  };
}
