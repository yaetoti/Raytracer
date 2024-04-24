#pragma once

#include "WindowEvent.h"

namespace Flame {
  struct ResizeWindowEvent final : WindowEvent {
    ResizeWindowEvent(uint32_t width, uint32_t height, uint32_t surfaceWidth, uint32_t surfaceHeight)
    : WindowEvent(WindowEventType::RESIZE)
    , width(width)
    , height(height)
    , surfaceWidth(surfaceWidth)
    , surfaceHeight(surfaceHeight) {
    }

    uint32_t width;
    uint32_t height;
    uint32_t surfaceWidth;
    uint32_t surfaceHeight;
  };
}
