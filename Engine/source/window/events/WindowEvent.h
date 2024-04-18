#pragma once

enum class WindowEventType {
  RESIZE,
  KEY,
  MOUSE_BUTTON,
  MOUSE_MOVE,
  COUNT
};

struct WindowEvent {
  virtual ~WindowEvent() = default;

  WindowEventType type;

protected:
  WindowEvent(WindowEventType type);
};
