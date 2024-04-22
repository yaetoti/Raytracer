#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "Flame/math/Ray.h"
#include "Flame/utils/EventDispatcher.h"
#include "Flame/window/events/WindowEvent.h"

namespace Flame {
  struct Camera final : EventListener<WindowEvent> {
    Camera(size_t width, size_t height, float fov, float near, float far);

    const Ray& GetRay(size_t x, size_t y);
    void GenerateRays();
    void HandleEvent(const WindowEvent& e) override;

  private:

  private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::quat m_rotation;

    glm::mat4 m_projection;
    glm::mat4 m_iProjection;
    glm::mat4 m_view;
    glm::mat4 m_iView;

    std::vector<Ray> m_rays;
    size_t m_bufferWidth = 800;
    size_t m_bufferHeight = 600;
    bool m_dirty = false;
  };  
}
