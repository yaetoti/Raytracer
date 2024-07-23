#pragma once
#include "Flame/window/Window.h"

namespace Flame {
  struct DxRenderer final {
    explicit DxRenderer(Window* window);

    void Render();
    void Resize(uint32_t width, uint32_t height);

  public:
    Window* m_window;
  };
}
