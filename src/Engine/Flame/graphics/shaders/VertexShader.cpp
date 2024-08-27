#include "VertexShader.h"
#include <cassert>
#include <d3dcompiler.h>

#include "Flame/graphics/DxContext.h"

namespace Flame {
  ID3DBlob* VertexShader::GetBlob() const {
    return m_blob.Get();
  }

  ID3D11VertexShader* VertexShader::GetShader() const {
    return m_shader.Get();
  }

  void VertexShader::Reset() {
    m_shader.Reset();
    m_blob.Reset();
  }

  void VertexShader::Init(const std::wstring& path) {
    HRESULT result;
    ComPtr<ID3DBlob> errorBlob;

    // Blob
    result = D3DCompileFromFile(
      path.c_str(),
      nullptr,
      D3D_COMPILE_STANDARD_FILE_INCLUDE,
      "VSMain",
      "vs_5_0",
      D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
      0,
      m_blob.ReleaseAndGetAddressOf(),
      errorBlob.ReleaseAndGetAddressOf()
    );
    assert(SUCCEEDED(result));
    if (FAILED(result)) {
      printf("%s\n", static_cast<LPCWSTR>(errorBlob->GetBufferPointer()));
      return;
    }

    // Shader
    result = DxContext::Get()->d3d11Device->CreateVertexShader(
      m_blob->GetBufferPointer(),
      m_blob->GetBufferSize(),
      nullptr,
      m_shader.ReleaseAndGetAddressOf()
    );
    assert(SUCCEEDED(result));
    if (FAILED(result)) {
      return;
    }
  }
}
