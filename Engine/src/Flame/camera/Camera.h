#pragma once

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace Flame {
  struct Camera final {
    Camera(float fov, float near, float far);

  private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::quat m_rotation;
  };  
}
