#pragma once
#include "IHitable.h"

namespace Flame {
  struct Triangle final : IHitable {
    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal, const Material* material);

    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const override;

    glm::vec3 point1;
    glm::vec3 point2;
    glm::vec3 point3;
    glm::vec3 normal;
    const Material* material;
  };
}
