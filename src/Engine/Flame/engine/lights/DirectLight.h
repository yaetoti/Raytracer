#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Flame {
  struct DirectLight final {
    struct ShaderData final {
      float direction[4];
      float radiance[3];
      float solidAngle;
    };

  public:
    ShaderData ToShaderData() const {
      return ShaderData {
        { direction.x, direction.y, direction.z, 0.0f },
        { radiance.x, radiance.y, radiance.z },
        solidAngle,
      };
    }

  public:
    glm::vec3 direction;
    glm::vec3 radiance;
    float solidAngle;
  };
}
