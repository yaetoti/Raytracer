#pragma once

#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/graphics/DxContext.h"

namespace Flame {
  struct PostProcess final {
    void Init() {
      m_resolveVertexShader.Init(L"Assets/Shaders/resolve.hlsl", nullptr, 0);
      m_resolvePixelShader.Init(L"Assets/Shaders/resolve.hlsl");
    }

    void Cleanup() {
      m_resolveVertexShader.Reset();
      m_resolvePixelShader.Reset();
    }

    void Resolve(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst) {
      ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
      dc->OMSetRenderTargets(1, &dst, nullptr);
      dc->PSSetShaderResources(0, 1, &src);
      dc->VSSetShader(m_resolveVertexShader.GetShader(), nullptr, 0);
      dc->PSSetShader(m_resolvePixelShader.GetShader(), nullptr, 0);
      dc->IASetInputLayout(nullptr);
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
    VertexShader m_resolveVertexShader;
    PixelShader m_resolvePixelShader;
  };
}
