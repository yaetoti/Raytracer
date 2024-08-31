#include "AabbOld.h"

namespace Flame {
  AabbOld::AabbOld(glm::vec3 min, glm::vec3 max)
  : m_min(min)
  , m_max(max) {
  }

  bool AabbOld::Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const {
    for (int d = 0; d < 3; ++d) {
      float t1 = (m_min[d] - r.origin[d]) / r.direction[d];
      float t2 = (m_max[d] - r.origin[d]) / r.direction[d];
      tMin = glm::max(glm::min(t1, t2), tMin);
      tMax = glm::min(glm::max(t1, t2), tMax);
      if (tMin > tMax) {
        return false;
      }
    }

    record.time = tMin;
    record.hitable = const_cast<AabbOld*>(this);
    return true;
  }

  bool AabbOld::Intersects(const AabbOld& other) const {
    return m_min.x <= other.m_max.x && m_max.x >= other.m_min.x
      && m_min.y <= other.m_max.y && m_max.y >= other.m_min.y
      && m_min.z <= other.m_max.z && m_max.z >= other.m_min.z;
  }

  void AabbOld::Union(const AabbOld& other) {
    for (int dim = 0; dim < 3; ++dim) {
      m_min[dim] = glm::min(other.m_min[dim], m_min[dim]);
      m_max[dim] = glm::max(other.m_max[dim], m_max[dim]);
    }
  }

  void AabbOld::Expand(const glm::vec3& size) {
    m_min -= size;
    m_max += size;
  }

  const glm::vec3& AabbOld::Min() const {
    return m_min;
  }

  const glm::vec3& AabbOld::Max() const {
    return m_max;
  }

  glm::vec3 AabbOld::Centroid() const {
    return (m_min + m_max) * 0.5f;
  }

  glm::vec3 AabbOld::Size() const {
    return m_max - m_min;
  }

  uint32_t AabbOld::GetBiggestSideIndex() const {
    glm::vec3 size = Size();

    int axis = 0;
    if (size[axis] < size.y) {
      axis = 1;
    }
    if (size[axis] < size.z) {
      axis = 2;
    }

    return axis;
  }

  void AabbOld::SetMin(const glm::vec3& min) {
    m_min = min;
  }

  void AabbOld::SetMax(const glm::vec3& max) {
    m_max = max;
  }

  AabbOld AabbOld::Empty() {
    return AabbOld(
      glm::vec3(std::numeric_limits<float>::infinity()),
      glm::vec3(-std::numeric_limits<float>::infinity())
    );
  }

  std::ostream& operator<<(std::ostream& out, const AabbOld& box) {
    return out << "AabbOld { " << box.m_min << ", " << box.m_max << " }";
  }
}
