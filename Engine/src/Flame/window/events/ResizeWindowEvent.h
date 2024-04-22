#pragma once

#include "WindowEvent.h"

namespace Flame {
  struct ResizeWindowEvent final : WindowEvent {
    ResizeWindowEvent(uint32_t width, uint32_t height)
    : WindowEvent(WindowEventType::RESIZE)
    , width(width)
    , height(height) {
    }

    uint32_t width;
    uint32_t height;
  };
}
