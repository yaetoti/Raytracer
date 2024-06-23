#include "Sphere.h"

namespace Flame {
  Sphere::Sphere(const glm::vec3& center, const float& radius, const Material* material)
  : m_center(center)
  , m_radius(radius)
  , m_material(material) {
    UpdateAabb();
  }

  bool Sphere::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    if (!m_bound.Hit(r, record, tMin, tMax)) {
      return false;
    }

    glm::vec3 oc = r.origin - m_center;

    float b = glm::dot(oc, r.direction);
    float c = glm::dot(oc, oc) - m_radius * m_radius;
    float d = b * b - c;

    if (d > 0) {
      float t = -b - sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - m_center) / m_radius;
        record.material = m_material;
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }

      t = -b + sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - m_center) / m_radius;
        record.material = m_material;
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }
    }

    return false;
  }

  const glm::vec3& Sphere::Center() const {
    return m_center;
  }
}
