#include "PostProcess.h"

#include <Flame/utils/PtrProxy.h>

namespace Flame {
  void PostProcess::Init() {
    m_pipeline.Init(kShaderPath, ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    m_buffer.Init();
  }

  void PostProcess::Cleanup() {
    m_pipeline.Reset();
    m_buffer.Reset();
  }

  float PostProcess::GetEvFactor() const {
    return m_buffer.data.evFactor;
  }

  void PostProcess::SetEvFactor(float evFactor) {
    m_buffer.data.evFactor = evFactor;
    m_bufferDirty = true;
  }

  void PostProcess::Resolve(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst) const {
    if (m_bufferDirty) {
      m_bufferDirty = false;
      m_buffer.ApplyChanges();
    }

    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    m_pipeline.Bind();
    dc->OMSetRenderTargets(1, &dst, nullptr);
    dc->PSSetShaderResources(0, 1, &src);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->PSSetConstantBuffers(0, 1, m_buffer.GetAddressOf());
    dc->Draw(3, 0);

    //dc->OMSetRenderTargets(1, PtrProxy<ID3D11RenderTargetView*>(nullptr).Ptr(), nullptr);
    dc->PSSetShaderResources(0, 1, PtrProxy<ID3D11ShaderResourceView*>(nullptr).Ptr());
  }

  PostProcess* PostProcess::Get() {
    static PostProcess instance;
    return &instance;
  }
}
