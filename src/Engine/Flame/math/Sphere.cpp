#include "Sphere.h"

#include "MathUtils.h"

namespace Flame {
  Sphere::Sphere(const glm::vec3& center, const float& radius)
  : center(center)
  , radius(radius) {
  }

  bool Sphere::Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const {
    glm::vec3 oc = r.origin - center;

    float b = glm::dot(oc, r.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float d = b * b - c;

    if (d > 0) {
      float t = -b - sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - center) / radius;
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }

      t = -b + sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - center) / radius;
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }
    }

    return false;
  }

  std::ostream& operator<<(std::ostream& out, const Sphere& s) {
    out << "{ C: " << s.center << ", R: " << s.radius << " }";
    return out;
  }
}
