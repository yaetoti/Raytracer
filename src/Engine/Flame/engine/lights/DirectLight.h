#pragma once

#include <glm/glm.hpp>

namespace Flame {
  struct DirectLight final {
    struct ShaderData final {
      glm::mat4 viewMat;
      glm::mat4 projectionMat;
      glm::vec4 direction;
      glm::vec3 radiance;
      float solidAngle;
    };

  public:
    ShaderData ToShaderData() const {
      return ShaderData {
        viewMat,
        projectionMat,
        glm::vec4(direction, 0.0),
        radiance,
        solidAngle,
      };
    }

  public:
    glm::mat4 viewMat;
    glm::mat4 projectionMat;
    glm::vec3 direction;
    glm::vec3 radiance;
    float solidAngle;
  };
}
