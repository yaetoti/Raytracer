#include "Sphere.h"

#include "MathUtils.h"

namespace Flame {
  Sphere::Sphere(const glm::vec3& center, const float& radius)
  : m_center(center)
  , m_radius(radius) {
  }

  bool Sphere::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
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
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }

      t = -b + sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - m_center) / m_radius;
        record.hitable = const_cast<Sphere*>(this);
        return true;
      }
    }

    return false;
  }

  const glm::vec3& Sphere::Center() const {
    return m_center;
  }

  void Sphere::SetCenter(const glm::vec3& center) {
    m_center = center;
  }

  float Sphere::Radius() const {
    return m_radius;
  }

  void Sphere::SetRadius(float radius) {
    m_radius = radius;
  }

  std::ostream& operator<<(std::ostream& out, const Sphere& s) {
    out << "{ C: " << s.m_center << ", R: " << s.m_radius << " }";
    return out;
  }
}
