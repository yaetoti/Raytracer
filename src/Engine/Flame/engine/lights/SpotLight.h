#pragma once

#include <glm/vec3.hpp>

namespace Flame {
  struct SpotLight final {
    struct ShaderData final {
      float position[4];
      float direction[4];
      float color[4];
      float intensity;

      float cutoffCosineInner;
      float cutoffCosineOuter;

      float constantFadeoff;
      float linearFadeoff;
      float quadraticFadeoff;

      float padding0[2];
    };

  public:
    ShaderData ToShaderData() const {
      return ShaderData {
        { position.x, position.y, position.z, 1.0f },
        { direction.x, direction.y, direction.z, 0.0f },
        { color.x, color.y, color.z, 1.0f },
        intensity,
        cutoffCosineInner,
        cutoffCosineOuter,
        constantFadeoff,
        linearFadeoff,
        quadraticFadeoff,
        { 0, 0 }
      };
    }

  public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;

    float cutoffCosineInner;
    float cutoffCosineOuter;

    float linearFadeoff;
    float constantFadeoff;
    float quadraticFadeoff;

    // Texture mask
  };
}
