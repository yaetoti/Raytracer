#pragma once

#include "WindowEvent.h"

struct MouseButtonWindowEvent final : WindowEvent {
    MouseButtonWindowEvent(bool isLeft, bool isPressed)
        : WindowEvent(WindowEventType::MOUSE_BUTTON)
        , isLeft(isLeft)
        , isPressed(isPressed) {

    }

    bool isLeft;
    bool isPressed;
};
