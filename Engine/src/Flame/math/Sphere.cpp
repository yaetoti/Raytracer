#include "Sphere.h"

namespace Flame {
  Sphere::Sphere(const glm::vec3& center, const float& radius, const IMaterial& material)
  : center(center)
  , radius(radius)
  , material(material) {
  }

  bool Sphere::Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const {
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
        record.material = &material;
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }

      t = -b + sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - center) / radius;
        record.material = &material;
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }
    }

    return false;
  }
}
