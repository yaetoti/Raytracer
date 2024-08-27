#pragma once

#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/graphics/DxContext.h"

namespace Flame {
  struct PostProcess final {
    void Init() {
      m_pipeline.Init(kShaderPath, ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    }

    void Cleanup() {
      m_pipeline.Reset();
    }

    void Resolve(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst) {
      ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
      dc->OMSetRenderTargets(1, &dst, nullptr);
      dc->PSSetShaderResources(0, 1, &src);
      m_pipeline.Bind();
      dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      dc->Draw(3, 0);
    }

    static PostProcess* Get() {
      static PostProcess instance;
      return &instance;
    }

  private:
    PostProcess() = default;

  private:
    ShaderPipeline m_pipeline;
    inline static const wchar_t* kShaderPath = L"Assets/Shaders/resolve.hlsl";
  };
}
