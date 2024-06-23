#include "Aabb.h"

namespace Flame {
  Aabb::Aabb(glm::vec3 min, glm::vec3 max)
  : m_min(min)
  , m_max(max) {
  }

  bool Aabb::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    for (int d = 0; d < 3; ++d) {
      float t1 = (m_min[d] - r.origin[d]) / r.direction[d];
      float t2 = (m_max[d] - r.origin[d]) / r.direction[d];
      tMin = glm::max(glm::min(t1, t2), tMin);
      tMax = glm::min(glm::max(t1, t2), tMax);
      if (tMin > tMax) {
        return false;
      }
    }

    // TODO: No need now
    // record.time = tMin;
    // record.hitable = const_cast<Aabb*>(this);
    return true;
  }

  bool Aabb::Intersects(const Aabb& other) const {
    return m_min.x <= other.m_max.x && m_max.x >= other.m_min.x
      && m_min.y <= other.m_max.y && m_max.y >= other.m_min.y
      && m_min.z <= other.m_max.z && m_max.z >= other.m_min.z;
  }

  void Aabb::Union(const Aabb& other) {
    for (int dim = 0; dim < 3; ++dim) {
      m_min[dim] = glm::min(other.m_min[dim], m_min[dim]);
      m_max[dim] = glm::max(other.m_max[dim], m_max[dim]);
    }
  }

  void Aabb::Expand(const glm::vec3& size) {
    m_min -= size;
    m_max += size;
  }

  const glm::vec3& Aabb::Min() const {
    return m_min;
  }

  const glm::vec3& Aabb::Max() const {
    return m_max;
  }

  glm::vec3 Aabb::Centroid() const {
    return (m_min + m_max) * 0.5f;
  }

  glm::vec3 Aabb::Size() const {
    return m_max - m_min;
  }

  uint32_t Aabb::GetBiggestSideIndex() const {
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

  void Aabb::SetMin(const glm::vec3& min) {
    m_min = min;
  }

  void Aabb::SetMax(const glm::vec3& max) {
    m_max = max;
  }

  Aabb Aabb::Empty() {
    return Aabb(
      glm::vec3(std::numeric_limits<float>::infinity()),
      glm::vec3(-std::numeric_limits<float>::infinity())
    );
  }

  std::ostream& operator<<(std::ostream& out, const Aabb& box) {
    return out << "Aabb { " << box.m_min << ", " << box.m_max << " }";
  }
}
