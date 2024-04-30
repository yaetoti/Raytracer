#pragma once

#include "WindowEvent.h"

namespace Flame {
  struct MouseScrollWindowEvent final : WindowEvent {
    MouseScrollWindowEvent(float xCursor, float yCursor, float delta)
    : WindowEvent(WindowEventType::MOUSE_SCROLL)
    , xCursor(xCursor)
    , yCursor(yCursor)
    , delta(delta) {
    }

    float xCursor;
    float yCursor;
    float delta;
  };
}
