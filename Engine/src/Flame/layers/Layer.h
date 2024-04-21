#pragma once

#include "Flame/render/RenderSurface.h"
#include "Flame/utils/EventDispatcher.h"
#include "Flame/window/events/WindowEvent.h"

namespace Flame {
  struct Layer : EventListener<WindowEvent> {
    ~Layer() override = default;

    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Render(RenderSurface& surface) {}
    virtual void Cleanup() {}
    void HandleEvent(const WindowEvent& e) override {}
  };
}
