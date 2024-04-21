#pragma once

#include "IHitable.h"
#include "Ray.h"
#include <iostream>

namespace Flame {
  struct IMaterial;

  struct Sphere final : IHitable {
    explicit Sphere(const glm::vec3& center, const float& radius, const IMaterial& material);
    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const override;

    friend std::ostream& operator<<(std::ostream& out, const Sphere& s) {
      out << "{ C: " << s.center << ", R: " << s.radius << " }";
      return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const Sphere& s) {
      out << L"{ C: " << s.center << L", R: " << s.radius << L" }";
      return out;
    }

  public:
    glm::vec3 center;
    float radius;
    const IMaterial& material;
  };
}
