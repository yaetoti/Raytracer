#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Flame {
  struct DirectLight final {
    struct ShaderData final {
      glm::vec4 direction;
      glm::vec3 radiance;
      float solidAngle;
    };

  public:
    ShaderData ToShaderData() const {
      return ShaderData {
        glm::vec4(direction, 0.0),
        radiance,
        solidAngle,
      };
    }

  public:
    glm::vec3 direction;
    glm::vec3 radiance;
    float solidAngle;
  };
}
