#pragma once

#include "IHitable.h"
#include "Ray.h"
#include <ostream>

namespace Flame {
  struct Material;

  struct Sphere final : IHitable {
    explicit Sphere(const glm::vec3& center, const float& radius);
    bool Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const override;

    friend std::ostream& operator<<(std::ostream& out, const Sphere& s);

  public:
    glm::vec3 center;
    float radius;
  };
}
