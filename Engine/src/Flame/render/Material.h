#pragma once

#include "Flame/math/Ray.h"

namespace Flame {
  // TODO Primitive::IsLightHittable

  struct Primitive final {
    bool Hit();

    glm::vec3 position;
    glm::mat4 modelMatrix;
    glm::mat4 modelMatrixInversed;
  };

  struct Material final {
    glm::vec3 albedo { 1.0f };
    float diffuse = 0.0f;
    float specular = 0.0f;
    float specularExponent = 0.0f;

    float roughness = 1.0f;
    float metallic = 0.0f;
    // TODO Not used
    glm::vec3 emissionColor { 0.0f };
    float emissionStrength = 0.0f;
  };

  struct PointLight final {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    float linearFadeoff;
    float constantFadeoff;
    float quadraticFadeoff;
  };

  struct DirectLight final {
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
  };

  struct SpotLight final {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float cutoffCosineInner;
    float cutoffCosineOuter;

    float linearFadeoff;
    float constantFadeoff;
    float quadraticFadeoff;
  };
}
