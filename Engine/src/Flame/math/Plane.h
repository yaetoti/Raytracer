#pragma once

#include "IHitable.h"

namespace Flame {
  struct Plane final : IHitable {
    Plane(const glm::vec3& position, const glm::vec3& normal, int materialId)
    : position(position)
    , normal(normal)
    , materialId(materialId) {
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      float t = glm::dot(normal, position - r.origin) / glm::dot(normal, r.direction);
      if (t > tMin && t < tMax) {
        record.point = r.AtParameter(t);
        record.normal = normal;
        record.time = t;
        record.materialId = materialId;
        record.hitable = const_cast<Plane*>(this);
        return true;
      }

      return false;
    }

    glm::vec3 position;
    glm::vec3 normal;
    int materialId;
  };  
}
