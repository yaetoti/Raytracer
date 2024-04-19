#pragma once

#include "Flame/Core.h"

enum class FLAME_API WindowEventType {
  RESIZE,
  KEY,
  MOUSE_BUTTON,
  MOUSE_MOVE,
  COUNT
};

struct FLAME_API WindowEvent {
  virtual ~WindowEvent() = default;

  WindowEventType type;

protected:
  explicit WindowEvent(WindowEventType type);
};
