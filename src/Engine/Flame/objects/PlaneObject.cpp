#include "PlaneObject.h"

namespace Flame {
  PlaneObject::PlaneObject(const Plane& plane, int materialId)
  : m_plane(plane)
  , m_materialId(materialId) {
  }

  bool PlaneObject::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    if (m_plane.Hit(r, record, tMin, tMax)) {
      record.materialId = m_materialId;
      return true;
    }

    return false;
  }
}
