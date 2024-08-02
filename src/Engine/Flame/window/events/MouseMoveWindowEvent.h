#pragma once

#include "WindowEvent.h"

namespace Flame {
  struct MouseMoveWindowEvent final : WindowEvent {
    MouseMoveWindowEvent(float xCursor, float yCursor)
    : WindowEvent(WindowEventType::MOUSE_MOVE)
    , xCursor(xCursor)
    , yCursor(yCursor) {
    }

    float xCursor;
    float yCursor;
  };
}
