#pragma once

#include "events/KeyWindowEvent.h"
#include "events/MouseButtonWindowEvent.h"
#include "events/MouseMoveWindowEvent.h"
#include "events/MouseScrollWindowEvent.h"
#include "Flame/utils/EventDispatcher.h"

namespace Flame {
  struct InputSystem final : EventListener<WindowEvent> {
    InputSystem();

    void Update();
    void HandleEvent(const WindowEvent& e) override;

    bool IsKeyPressed(size_t key) const;
    bool IsMouseButtonPressed(MouseButton button) const;
    float GetCursorX() const;
    float GetLastCursorX() const;
    float GetCursorY() const;
    float GetLastCursorY() const;
    std::pair<float, float> GetCursorPos() const;
    std::pair<float, float> GetLastCursorPos() const;
    float GetScrollDelta() const;

  private:
    void HandleKeyEvent(const KeyWindowEvent& e);
    void HandleMouseButtonEvent(const MouseButtonWindowEvent& e);
    void HandleMouseMoveEvent(const MouseMoveWindowEvent& e);
    void HandleMouseScrollEvent(const MouseScrollWindowEvent& e);

  public:
    constexpr static size_t kKeyCount = 255;
    constexpr static size_t kMouseButtonCount = 5;

  private:
    bool m_keyMap[kKeyCount];
    bool m_mouseButtonMap[kMouseButtonCount];
    float m_cursorX;
    float m_cursorY;
    float m_lastCursorX;
    float m_lastCursorY;
    float m_scrollDelta;
  };
}
