#include "ShaderPipeline.h"

namespace Flame {
  void ShaderPipeline::Init(const std::wstring& filename, ShaderType types) {
    // TODO: need to change order in 2 places if I'm gonna add another shader
    std::wstring* paths[] = {
      &m_vsFile,
      &m_hsFile,
      &m_dsFile,
      &m_gsFile,
      &m_psFile,
    };

    for (uint32_t i = 0, end = ARRAYSIZE(kFilenameTypes); i < end; ++i) {
      if (IsSet(types, kFilenameTypes[i])) {
        paths[i][0] = filename;
      } else {
        paths[i][0].clear();
      }
    }

    Compile();
  }

  void ShaderPipeline::Init(const std::wstring& vsFile, const std::wstring& psFile) {
    m_vsFile = vsFile;
    m_psFile = psFile;
    m_hsFile.clear();
    m_dsFile.clear();
    m_gsFile.clear();
    Compile();
  }

  void ShaderPipeline::Init(const std::wstring& vsFile, const std::wstring& hsFile, const std::wstring& dsFile, const std::wstring& gsFile, const std::wstring& psFile) {
    m_vsFile = vsFile;
    m_hsFile = hsFile;
    m_dsFile = dsFile;
    m_gsFile = gsFile;
    m_psFile = psFile;
    Compile();
  }

  void ShaderPipeline::CreateInputLayout(std::span<D3D11_INPUT_ELEMENT_DESC> descriptions) {
    assert(m_vertexShader.GetBlob());
    HRESULT result = DxContext::Get()->d3d11Device->CreateInputLayout(
      descriptions.data(),
      descriptions.size(),
      m_vertexShader.GetBlob()->GetBufferPointer(),
      m_vertexShader.GetBlob()->GetBufferSize(),
      m_inputLayout.ReleaseAndGetAddressOf()
    );
    assert(SUCCEEDED(result));
  }

  void ShaderPipeline::SetInputLayout(ID3D11InputLayout* layout) {
    m_inputLayout.Attach(layout);
  }

  void ShaderPipeline::SetInputLayout(ComPtr<ID3D11InputLayout> layout) {
    m_inputLayout = std::move(layout);
  }

  void ShaderPipeline::Compile() {
    m_vertexShader.Reset();
    m_hullShader.Reset();
    m_domainShader.Reset();
    m_geometryShader.Reset();
    m_pixelShader.Reset();

    if (!m_vsFile.empty()) {
      m_vertexShader.Init(m_vsFile);
    }
    if (!m_hsFile.empty()) {
      m_hullShader.Init(m_vsFile);
    }
    if (!m_dsFile.empty()) {
      m_domainShader.Init(m_vsFile);
    }
    if (!m_gsFile.empty()) {
      m_geometryShader.Init(m_vsFile);
    }
    if (!m_psFile.empty()) {
      m_pixelShader.Init(m_vsFile);
    }
  }

  void ShaderPipeline::Unbind() {
    auto dc = DxContext::Get()->d3d11DeviceContext;
    dc->IASetInputLayout(nullptr);
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
  }

  void ShaderPipeline::Reset() {
    m_vertexShader.Reset();
    m_hullShader.Reset();
    m_domainShader.Reset();
    m_geometryShader.Reset();
    m_pixelShader.Reset();
    m_inputLayout.Reset();
  }

  void ShaderPipeline::Bind() const {
    auto dc = DxContext::Get()->d3d11DeviceContext;
    dc->IASetInputLayout(m_inputLayout.Get());
    dc->VSSetShader(m_vertexShader.GetShader(), nullptr, 0);
    dc->HSSetShader(m_hullShader.GetShader(), nullptr, 0);
    dc->DSSetShader(m_domainShader.GetShader(), nullptr, 0);
    dc->GSSetShader(m_geometryShader.GetShader(), nullptr, 0);
    dc->PSSetShader(m_pixelShader.GetShader(), nullptr, 0);
  }
}
