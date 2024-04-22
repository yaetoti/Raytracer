#pragma once

#include "Flame/math/IHitable.h"

#include <memory>
#include <vector>
#include <Flame/math/Ray.h>
#include <Flame/render/RenderSurface.h>
#include <glm/glm.hpp>

#include "Flame/camera/Camera.h"
#include "Flame/utils/EventDispatcher.h"
#include "Flame/window/events/WindowEvent.h"

namespace Flame {
  struct Scene : EventListener<WindowEvent> {
    virtual ~Scene() override = default;

    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Cleanup() {}

    void Render(RenderSurface& surface, Camera& camera);
    void HandleEvent(const WindowEvent& e) override {}

  private:
    glm::vec3 Color(const Ray& ray, int depth);

  protected:
    std::vector<std::unique_ptr<IHitable>> m_hitables;
  };
}
