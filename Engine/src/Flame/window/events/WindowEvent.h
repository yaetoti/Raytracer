#pragma once

namespace Flame {
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
    explicit WindowEvent(WindowEventType type)
    : type(type) {
    }
  };
}
