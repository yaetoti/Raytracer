#pragma once

#include "WindowEvent.h"

struct ResizeWindowEvent final : WindowEvent {
  ResizeWindowEvent(size_t width, size_t height)
  : WindowEvent(WindowEventType::RESIZE)
  , width(width)
  , height(height) {
  }

  size_t width;
  size_t height;
};
