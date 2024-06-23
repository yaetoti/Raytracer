#pragma once

#include "Flame/math/IHitable.h"

namespace Flame {
  struct TriangleObject final : IHitable {
    TriangleObject(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal, const Material* material);

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

    glm::vec3 point1;
    glm::vec3 point2;
    glm::vec3 point3;
    glm::vec3 normal;
    const Material* material;
  };
}
