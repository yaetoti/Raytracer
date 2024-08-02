#pragma once

#include <ostream>
#include <glm/vec3.hpp>

#include "IHitable.h"

namespace Flame {
  struct AabbOld final : IHitable {
    explicit AabbOld(glm::vec3 min = glm::vec3(0.0f), glm::vec3 max = glm::vec3(0.0f));

    bool Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const override;

    bool Intersects(const AabbOld& other) const;

    void SetMin(const glm::vec3& min);
    void SetMax(const glm::vec3& max);
    void Union(const AabbOld& other);
    void Expand(const glm::vec3& size);

    const glm::vec3& Min() const;
    const glm::vec3& Max() const;
    glm::vec3 Centroid() const;
    glm::vec3 Size() const;
    uint32_t GetBiggestSideIndex() const;

    template <typename It>
    static AabbOld Union(It begin, It end) {
      glm::vec3 min(std::numeric_limits<float>::infinity());
      glm::vec3 max(-std::numeric_limits<float>::infinity());

      while (begin != end) {
        for (int dim = 0; dim < 3; ++dim) {
          min[dim] = glm::min(begin->Min()[dim], min[dim]);
          max[dim] = glm::max(begin->Max()[dim], max[dim]);
        }

        ++begin;
      }

      return AabbOld(min, max);
    }

    static AabbOld Empty();

    friend std::ostream& operator<<(std::ostream& out, const AabbOld& box);

  private:
    glm::vec3 m_min;
    glm::vec3 m_max;
  };

  struct IHitableWithBounds : IHitable {
    virtual const AabbOld& GetBound() const = 0;
  };
}

