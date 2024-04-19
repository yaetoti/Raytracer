#pragma once

#include "WindowEvent.h"

struct MouseMoveWindowEvent final : WindowEvent {
  MouseMoveWindowEvent(int deltaX, int deltaY)
  : WindowEvent(WindowEventType::MOUSE_MOVE)
  , deltaX(deltaX)
  , deltaY(deltaY) {
  }

  int deltaX;
  int deltaY;
};
