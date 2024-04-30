#pragma once

#include "IHitable.h"
#include "Ray.h"
#include <iostream>

#include "Aabb.h"

namespace Flame {
  struct Material;

  struct Sphere final : IHitable {
    explicit Sphere(const glm::vec3& center, const float& radius, const Material* material);
    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

    friend std::ostream& operator<<(std::ostream& out, const Sphere& s) {
      out << "{ C: " << s.center << ", R: " << s.radius << " }";
      return out;
    }

    friend std::wostream& operator<<(std::wostream& out, const Sphere& s) {
      out << L"{ C: " << s.center << L", R: " << s.radius << L" }";
      return out;
    }

    void UpdateAabb() {
      bound = GetSphereBox(*this);
    }

  private:
    // TODO remove
    static Aabb GetSphereBox(const Sphere& o) {
      glm::vec3 min;
      glm::vec3 max;

      for (int dim = 0; dim < 3; ++dim) {
        min[dim] = o.center[dim] - o.radius;
        max[dim] = o.center[dim] + o.radius;
      }

      return Aabb(min, max);
    }

  public:
    glm::vec3 center;
    float radius;
    const Material* material;
    Aabb bound; // TODO SceneObject, SOLID
  };
}
