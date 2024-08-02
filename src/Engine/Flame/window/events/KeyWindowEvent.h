#pragma once

#include "WindowEvent.h"
#include <Windows.h>

namespace Flame {
  struct KeyWindowEvent final : WindowEvent {
    KeyWindowEvent(WORD vkCode, bool isPressed, bool wasPressed)
    : WindowEvent(WindowEventType::KEY)
    , vkCode(vkCode)
    , isPressed(isPressed)
    , wasPressed(wasPressed) {
    }

    WORD vkCode;
    bool isPressed;
    bool wasPressed;
  };
}
