#pragma once

#include "Flame/engine/TransformSystem.h"
#include <glm/vec3.hpp>

namespace Flame {
  struct PointLight final {
    struct ShaderData final {
      glm::vec4 position;
      glm::vec3 radiance;
      float radius;
    };

  public:
    ShaderData ToShaderData() const {
      auto m = TransformSystem::Get()->At(parentTransformId)->transform.GetMat();

      return ShaderData {
        glm::vec4(position.x + m[3][0], position.y + m[3][1], position.z + m[3][2], 1.0f),
        radiance,
        radius,
      };
    }

  public:
    uint32_t parentTransformId;
    glm::vec3 position;
    glm::vec3 radiance;
    float radius;
  };
}
