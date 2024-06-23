#include "SphereObject.h"

#include "Flame/math/MathUtils.h"

namespace Flame {
  SphereObject::SphereObject(const Sphere& sphere, int materialId)
  : m_sphere(sphere)
  , m_materialId(materialId) {
    m_bounds = MathUtils::AabbFromSphere(m_sphere);
  }

  bool SphereObject::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    if (!m_bounds.Hit(r, record, tMin, tMax)) {
      return false;
    }

    if (m_sphere.Hit(r, record, tMin, tMax)) {
      record.materialId = m_materialId;
      return true;
    }

    return false;
  }
}
