#pragma once

#include "Flame/render/IMaterial.h"
#include <glm/glm.hpp>

namespace Flame {
  struct IHitable;

  struct HitRecord final {
    float time;
    glm::vec3 point;
    glm::vec3 normal;
    const IMaterial* material;
    IHitable* hitable;
  };
}
