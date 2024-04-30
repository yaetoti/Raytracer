#pragma once

#include <ostream>
#include <glm/vec3.hpp>

#include "IHitable.h"

namespace Flame {
  struct Aabb final : IHitable {
    explicit Aabb(glm::vec3 min = glm::vec3(0.0f), glm::vec3 max = glm::vec3(0.0f))
    // TODO Non-empty check for 2-axis triangles
    : m_min(min - 0.01f)
    , m_max(max + 0.01f) {
      Recalculate();
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      for (int d = 0; d < 3; ++d) {
        float t1 = (m_min[d] - r.origin[d]) / r.direction[d];
        float t2 = (m_max[d] - r.origin[d]) / r.direction[d];
        tMin = glm::max(glm::min(t1, t2), tMin);
        tMax = glm::min(glm::max(t1, t2), tMax);
        if (tMin >= tMax) {
          return false;
        }
      }

      // TODO: No need now
      // record.time = tMin;
      // record.hitable = const_cast<Aabb*>(this);
      return true;
    }

    bool Intersects(const Aabb& other) const {
      return m_min.x <= other.m_max.x && m_max.x >= other.m_min.x
        && m_min.y <= other.m_max.y && m_max.y >= other.m_min.y
        && m_min.z <= other.m_max.z && m_max.z >= other.m_min.z;
    }

    void Union(const Aabb& other) {
      for (int dim = 0; dim < 3; ++dim) {
        m_min[dim] = glm::min(other.m_min[dim], m_min[dim]);
        m_max[dim] = glm::max(other.m_max[dim], m_max[dim]);
      }
    }

    const glm::vec3& Min() const {
      return m_min;
    }

    const glm::vec3& Max() const {
      return m_max;
    }

    const glm::vec3& Centroid() const {
      return m_centroid;
    }

    const glm::vec3& Size() const {
      return m_size;
    }

    uint32_t GetBiggestSideIndex() const {
      int axis = 0;
      if (m_size[axis] < m_size.y) {
        axis = 1;
      }
      if (m_size[axis] < m_size.z) {
        axis = 2;
      }

      return axis;
    }

    void SetMin(const glm::vec3& min) {
      m_min = min;
      Recalculate();
    }

    void SetMax(const glm::vec3& max) {
      m_max = max;
      Recalculate();
    }

    template <typename It>
    static Aabb Union(It begin, It end) {
      glm::vec3 min(std::numeric_limits<float>::infinity());
      glm::vec3 max(-std::numeric_limits<float>::infinity());

      while (begin != end) {
        for (int dim = 0; dim < 3; ++dim) {
          min[dim] = glm::min(begin->Min()[dim], min[dim]);
          max[dim] = glm::max(begin->Max()[dim], max[dim]);
        }

        ++begin;
      }

      return Aabb(min, max);
    }

    static Aabb EmptyIntersection() {
      return Aabb(
        glm::vec3(std::numeric_limits<float>::infinity()),
        glm::vec3(-std::numeric_limits<float>::infinity())
      );
    }

    friend std::ostream& operator<<(std::ostream& out, const Aabb& box) {
      return out << "Aabb { " << box.m_min << ", " << box.m_max << " }";
    }

  private:
    void Recalculate() {
      m_centroid = (m_min + m_max) * 0.5f;
      m_size = m_max - m_min;
    }

  private:
    glm::vec3 m_min;
    glm::vec3 m_max;
    // Cached data TODO Aabb should be lightweight. Mediator should decide whether to cache it or not
    glm::vec3 m_centroid;
    glm::vec3 m_size;
  };

  struct IHitableWithBounds : IHitable {
    virtual const Aabb& GetBound() const = 0;
  };
}

