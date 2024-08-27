#pragma once
#include <cassert>
#include <span>
#include <Flame/graphics/DxContext.h>
#include <Flame/graphics/shaders/DomainShader.h>
#include <Flame/graphics/shaders/GeometryShader.h>
#include <Flame/graphics/shaders/HullShader.h>
#include <Flame/graphics/shaders/PixelShader.h>
#include <Flame/graphics/shaders/VertexShader.h>

#include "ShaderType.h"

namespace Flame {
  // Wrapper around all shaders in pipeline. Usually we use them together, so grouping makes sense
  struct ShaderPipeline final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    // Init filenames and compile
    void Init(const std::wstring& filename, ShaderType types);
    void Init(const std::wstring& vsFile, const std::wstring& psFile);
    void Init(const std::wstring& vsFile, const std::wstring& hsFile, const std::wstring& dsFile, const std::wstring& gsFile, const std::wstring& psFile);
    void CreateInputLayout(std::span<D3D11_INPUT_ELEMENT_DESC> descriptions);
    void SetInputLayout(ID3D11InputLayout* layout);
    void SetInputLayout(ComPtr<ID3D11InputLayout> layout);
    // Compile shaders from existing filenames
    void Compile();
    void Reset();

    void Bind() const;
    static void Unbind();

  private:
    VertexShader m_vertexShader;
    HullShader m_hullShader;
    DomainShader m_domainShader;
    GeometryShader m_geometryShader;
    PixelShader m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    std::wstring m_vsFile;
    std::wstring m_hsFile;
    std::wstring m_dsFile;
    std::wstring m_gsFile;
    std::wstring m_psFile;

    inline static const ShaderType kFilenameTypes[] = {
      ShaderType::VERTEX_SHADER,
      ShaderType::HULL_SHADER,
      ShaderType::DOMAIN_SHADER,
      ShaderType::GEOMETRY_SHADER,
      ShaderType::PIXEL_SHADER,
    };
  };
}
