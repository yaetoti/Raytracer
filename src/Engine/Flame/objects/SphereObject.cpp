#include "SphereObject.h"

#include "Flame/math/MathUtils.h"

namespace Flame {
  SphereObject::SphereObject(const Sphere& sphere, int materialId)
  : m_sphere(sphere)
  , m_materialId(materialId) {
  }

  bool SphereObject::Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const {
    AabbOld bounds = MathUtils::AabbFromSphere(m_sphere);
    if (!bounds.Hit(r, record, tMin, tMax)) {
      return false;
    }

    if (m_sphere.Hit(r, record, tMin, tMax)) {
      record.materialId = m_materialId;
      return true;
    }

    return false;
  }
}
