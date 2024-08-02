#pragma once

#include "IHitable.h"

namespace Flame {
  struct Plane final : IHitable {
    explicit Plane(const glm::vec3& position, const glm::vec3& normal);

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

    glm::vec3 position;
    glm::vec3 normal;
  };  
}
