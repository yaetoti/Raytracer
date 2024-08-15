#pragma once

#include "Flame/math/Ray.h"

namespace Flame {
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
}
