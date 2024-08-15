#pragma once

#include <glm/vec3.hpp>

namespace Flame {
  struct PointLight final {
    struct ShaderData final {
      float position[4];
      float color[4];
      float intensity;

      float constantFadeoff;
      float linearFadeoff;
      float quadraticFadeoff;
    };

  public:
    ShaderData ToShaderData() const {
      return ShaderData {
        { position.x, position.y, position.z, 1.0f },
        { color.x, color.y, color.z, 1.0f },
        intensity,
        constantFadeoff,
        linearFadeoff,
        quadraticFadeoff
      };
    }

  public:
    uint32_t parentTransformId;
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    float constantFadeoff;
    float linearFadeoff;
    float quadraticFadeoff;
  };
}
