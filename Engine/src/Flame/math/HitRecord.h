#pragma once

#include "Flame/render/Material.h"
#include <glm/glm.hpp>

namespace Flame {
  struct IHitable;

  struct HitRecord final {
    float time;
    glm::vec3 point;
    glm::vec3 normal;
    IHitable* hitable;
    uint32_t materialId;

    const Material* material;
  };
}
