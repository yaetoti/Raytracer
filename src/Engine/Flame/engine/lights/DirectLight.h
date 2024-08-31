#pragma once

#include <glm/vec3.hpp>

namespace Flame {
  struct DirectLight final {
    struct ShaderData final {
      float direction[4];
      float color[4];
      float intensity;

      float padding0[3];
    };

  public:
    ShaderData ToShaderData() const {
      return ShaderData {
        { direction.x, direction.y, direction.z, 0.0f },
        { color.x, color.y, color.z, 1.0f },
        intensity,
        { 0, 0, 0 }
      };
    }

  public:
    glm::vec3 color;
    glm::vec3 direction;
    float intensity;
  };
}
