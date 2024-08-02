#pragma once

#include <glm/glm.hpp>

namespace Flame {
  struct HitRecord final {
    float time;
    glm::vec3 point;
    glm::vec3 normal;
    void* hitable;
  };
}
