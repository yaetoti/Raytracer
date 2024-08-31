#pragma once

#include <glm/glm.hpp>

namespace Flame {
  struct IHitable;

  struct HitRecordOld final {
    float time;
    glm::vec3 point;
    glm::vec3 normal;
    IHitable* hitable;
    int materialId;
  };
}
