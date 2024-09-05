#include "PixelShader.h"

#include <cassert>
#include <d3dcompiler.h>

#include "Flame/graphics/DxContext.h"

namespace Flame {
  ID3DBlob* PixelShader::GetBlob() const {
    return m_blob.Get();
  }

  ID3D11PixelShader* PixelShader::GetShader() const {
    return m_shader.Get();
  }

  void PixelShader::Reset() {
    m_shader.Reset();
    m_blob.Reset();
  }

  void PixelShader::Init(const std::wstring& path) {
    HRESULT result;
    ComPtr<ID3DBlob> errorBlob;

    // Blob
    result = D3DCompileFromFile(
      path.c_str(),
      nullptr,
      D3D_COMPILE_STANDARD_FILE_INCLUDE,
      "PSMain",
      "ps_5_0",
      D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
      0,
      m_blob.GetAddressOf(),
      errorBlob.GetAddressOf()
    );
    if (FAILED(result)) {
      printf("%s\n", static_cast<LPCWSTR>(errorBlob->GetBufferPointer()));
      assert(SUCCEEDED(result));
      return;
    }

    // Shader
    result = DxContext::Get()->d3d11Device->CreatePixelShader(
      m_blob->GetBufferPointer(),
      m_blob->GetBufferSize(),
      nullptr,
      m_shader.GetAddressOf()
    );
    assert(SUCCEEDED(result));
    if (FAILED(result)) {
      return;
    }
  }
}
