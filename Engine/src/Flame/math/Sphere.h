#pragma once

#include "IHitable.h"
#include "Ray.h"
#include <ostream>

namespace Flame {
  struct Material;

  struct Sphere final : IHitable {
    explicit Sphere(const glm::vec3& center, const float& radius);
    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

    const glm::vec3& Center() const;
    float Radius() const;

    void SetCenter(const glm::vec3& center);
    void SetRadius(float radius);

    friend std::ostream& operator<<(std::ostream& out, const Sphere& s);

  private:
    glm::vec3 m_center;
    float m_radius;
  };
}
