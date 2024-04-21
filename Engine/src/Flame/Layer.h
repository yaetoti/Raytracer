#pragma once

#include "render/RenderSurface.h"
#include "window/events/WindowEvent.h"
#include "utils/EventDispatcher.h"

namespace Flame {
  struct Layer : EventListener<WindowEvent> {
    ~Layer() override = default;

    virtual void Initialize() {}
    virtual void Update() {}
    virtual void Render(RenderSurface& surface) {}
    virtual void Cleanup() {}
    void HandleEvent(const WindowEvent& e) override {}
  };
}
