#pragma once

#include "WindowEvent.h"
#include <WinBase.h>

struct KeyWindowEvent final : WindowEvent {
  KeyWindowEvent(WORD vkCode, bool isPressed, bool wasPressed, WORD repeatCount, WORD scanCode)
  : WindowEvent(WindowEventType::KEY)
  , vkCode(vkCode)
  , scanCode(scanCode)
  , isPressed(isPressed)
  , wasPressed(wasPressed)
  , repeatCount(repeatCount) {
  }

  WORD vkCode;
  WORD scanCode;
  bool isPressed;
  bool wasPressed;
  WORD repeatCount;
};
