#include "DxRenderer.h"

namespace Flame {
  DxRenderer::DxRenderer(Window* window)
  : m_window(window) {
  }

  void DxRenderer::Render() {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    ID3D11RenderTargetView* targetView = m_window->GetTargetView();

    float clearColor[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
    dc->ClearRenderTargetView(targetView, clearColor);

    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //dc->IASetInputLayout();
    //dc->IASetVertexBuffers();
    //dc->VSSetShader();
    //dc->PSSetShader();

    // DRAW

  }

  void DxRenderer::Resize(uint32_t width, uint32_t height) {

  }
}
