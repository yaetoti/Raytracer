#include "Plane.h"

namespace Flame {
  Plane::Plane(const glm::vec3& position, const glm::vec3& normal)
  : position(position)
  , normal(normal) {
  }

  bool Plane::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    float t = glm::dot(normal, position - r.origin) / glm::dot(normal, r.direction);
    if (t > tMin && t < tMax) {
      record.point = r.AtParameter(t);
      record.normal = normal;
      record.time = t;
      record.hitable = const_cast<Plane*>(this);
      return true;
    }

    return false;
  }
}
