#pragma once

#include <array>

#include "Aabb.h"

namespace Flame {
  struct Triangle final : IHitableWithBounds {
    explicit Triangle(
      glm::vec3 point1 = glm::vec3(0.0f),
      glm::vec3 point2 = glm::vec3(0.0f),
      glm::vec3 point3 = glm::vec3(0.0f),
      glm::vec3 normal = glm::vec3(0.0f)
    );

    bool Hit(const Ray& r, HitRecord& record, float tMin = 0, float tMax = std::numeric_limits<float>::infinity()) const override;

    friend std::ostream& operator<<(std::ostream& os, const Triangle& obj) {
      return os << "Triangle { "
      << obj.points[0] << ", "
      << obj.points[1] << ", "
      << obj.points[2] << ", "
      << obj.normal << " }\n";
    }

    const Aabb& GetBound() const override {
      return bound;
    }

  public:
    std::array<glm::vec3, 3> points;
    glm::vec3 normal;
    Aabb bound;
  };
}
