#pragma once

#include "WindowEvent.h"

enum class MouseButton {
  LEFT,
  RIGHT,
  MIDDLE,
  X1,
  X2,
  COUNT
};

struct MouseButtonWindowEvent final : WindowEvent {
  MouseButtonWindowEvent(MouseButton button, bool isPressed, float xCursor, float yCursor)
  : WindowEvent(WindowEventType::MOUSE_BUTTON)
  , button(button)
  , isPressed(isPressed)
  , xCursor(xCursor)
  , yCursor(yCursor) {
  }

  MouseButton button;
  bool isPressed;
  float xCursor;
  float yCursor;
};
