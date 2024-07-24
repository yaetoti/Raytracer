#pragma once

#include "Flame/window/Window.h"
#include <wrl/client.h>

namespace Flame {
  struct DxRenderer final {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    explicit DxRenderer(Window* window);

    void Render();
    void Resize(uint32_t width, uint32_t height);

  private:
    Window* m_window;

    ComPtr<ID3DBlob> m_vertexShaderBlob;
    ComPtr<ID3DBlob> m_pixelShaderBlob;
  };
}
