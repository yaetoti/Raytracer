#include "InputSystem.h"

namespace Flame {
  InputSystem::InputSystem()
  : m_keyMap { }
  , m_mouseButtonMap { }
  , m_cursorX(0)
  , m_cursorY(0)
  , m_lastCursorX(0)
  , m_lastCursorY(0) {
  }

  void InputSystem::Update() {
    m_lastCursorX = m_cursorX;
    m_lastCursorY = m_cursorY;
  }

  void InputSystem::HandleEvent(const WindowEvent& e) {
      switch (e.type) {
        case WindowEventType::KEY:
          HandleKeyEvent(*dynamic_cast<const KeyWindowEvent*>(&e));
          return;
        case WindowEventType::MOUSE_BUTTON:
          HandleMouseButtonEvent(*dynamic_cast<const MouseButtonWindowEvent*>(&e));
          return;
        case WindowEventType::MOUSE_MOVE:
          HandleMouseMoveEvent(*dynamic_cast<const MouseMoveWindowEvent*>(&e));
          return;
        default: 
          break;
      }
  }

  bool InputSystem::IsKeyPressed(size_t key) const {
    return m_keyMap[key];
  }

  bool InputSystem::IsMouseButtonPressed(MouseButton button) const {
    return m_mouseButtonMap[static_cast<size_t>(button)];
  }

  float InputSystem::GetCursorX() const {
    return m_cursorX;
  }

  float InputSystem::GetLastCursorX() const {
    return m_lastCursorX;
  }

  float InputSystem::GetCursorY() const {
    return m_cursorY;
  }

  float InputSystem::GetLastCursorY() const {
    return m_lastCursorY;
  }

  std::pair<float, float> InputSystem::GetCursorPos() const {
    return std::make_pair(m_cursorX, m_cursorY);
  }

  std::pair<float, float> InputSystem::GetLastCursorPos() const {
    return std::make_pair(m_lastCursorX, m_lastCursorY);;
  }

  void InputSystem::HandleKeyEvent(const KeyWindowEvent& e) {
    m_keyMap[e.vkCode] = e.isPressed;
  }

  void InputSystem::HandleMouseButtonEvent(const MouseButtonWindowEvent& e) {
    m_mouseButtonMap[static_cast<size_t>(e.button)] = e.isPressed;
  }

  void InputSystem::HandleMouseMoveEvent(const MouseMoveWindowEvent& e) {
    m_cursorX = e.xCursor;
    m_cursorY = e.yCursor;
  }
}
