#pragma once

#include "IHitable.h"

namespace Flame {
  struct Plane final : IHitable {
    Plane(const glm::vec3& position, const glm::vec3& normal, const IMaterial& material)
    : position(position)
    , normal(normal)
    , material(material) {
    }

    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const override {
      float t = glm::dot(normal, position - r.origin) / glm::dot(normal, r.direction);
      if (t > tMin && t < tMax) {
        record.point = r.AtParameter(t);
        record.normal = normal;
        record.time = t;
        record.material = &material;
        return true;
      }

      return false;
    }

    glm::vec3 position;
    glm::vec3 normal;
    const IMaterial& material;
  };  
}
