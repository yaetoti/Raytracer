#pragma once

#include "Flame/math/Aabb.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/Sphere.h"

namespace Flame {
  struct SphereObject final : IHitable {
    explicit SphereObject(const Sphere& sphere, int materialId);
    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

  private:
    Sphere m_sphere;
    int m_materialId;
    Aabb m_bounds;
  };
}
