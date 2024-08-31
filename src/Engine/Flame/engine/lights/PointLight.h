#pragma once

#include "Flame/engine/TransformSystem.h"
#include <glm/vec3.hpp>

namespace Flame {
  struct PointLight final {
    struct ShaderData final {
      float position[4];
      float radiance[3];
      float radius;
    };

  public:
    ShaderData ToShaderData() const {
      auto m = TransformSystem::Get()->At(parentTransformId)->transform.GetMat();

      return ShaderData {
        { position.x + m[3][0], position.y + m[3][1], position.z + m[3][2], 1.0f },
        { radiance.x, radiance.y, radiance.z },
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
