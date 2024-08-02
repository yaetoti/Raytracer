#pragma once

#include <ostream>
#include <glm/glm.hpp>

#include "HitRecord.h"
#include "Flame/math/Ray.h"

namespace Flame {
  template <typename T>
  concept HasMinMax = requires(T t) {
      { t.Min() } -> std::convertible_to<glm::vec3>;
      { t.Max() } -> std::convertible_to<glm::vec3>;
  };

  struct Aabb final {
    explicit Aabb(glm::vec3 min = glm::vec3(0.0f), glm::vec3 max = glm::vec3(0.0f));

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const;

    bool Intersects(const Aabb& other) const;

    void SetMin(const glm::vec3& min);
    void SetMax(const glm::vec3& max);
    void Union(const Aabb& other);
    void Expand(const glm::vec3& size);

    const glm::vec3& Min() const;
    const glm::vec3& Max() const;
    glm::vec3 Centroid() const;
    glm::vec3 Size() const;
    uint32_t GetBiggestSideIndex() const;

    template <typename It>
    requires HasMinMax<typename std::iterator_traits<It>::value_type>
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

    static Aabb Union(const glm::vec3* begin, const glm::vec3* end) {
      glm::vec3 min(std::numeric_limits<float>::infinity());
      glm::vec3 max(-std::numeric_limits<float>::infinity());

      while (begin != end) {
        for (int dim = 0; dim < 3; ++dim) {
          min[dim] = glm::min((*begin)[dim], min[dim]);
          max[dim] = glm::max((*begin)[dim], max[dim]);
        }

        ++begin;
      }

      return Aabb(min, max);
    }

    static Aabb Empty();

    friend std::ostream& operator<<(std::ostream& out, const Aabb& box);

  private:
    glm::vec3 m_min;
    glm::vec3 m_max;
  };
}

