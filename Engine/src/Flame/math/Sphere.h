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
      out << "{ C: " << s.m_center << ", R: " << s.m_radius << " }";
      return out;
    }

    void UpdateAabb() {
      m_bound = GetSphereBox(*this);
    }

    const glm::vec3& Center() const;

    void SetCenter(const glm::vec3& center) {
      m_center = center;
      UpdateAabb();
    }

    float Radius() const {
      return m_radius;
    }

    void SetRadius(float radius) {
      m_radius = radius;
      UpdateAabb();
    }

  private:
    // TODO remove
    static Aabb GetSphereBox(const Sphere& o) {
      glm::vec3 min;
      glm::vec3 max;

      for (int dim = 0; dim < 3; ++dim) {
        min[dim] = o.m_center[dim] - o.m_radius;
        max[dim] = o.m_center[dim] + o.m_radius;
      }

      return Aabb(min, max);
    }

  private:
    glm::vec3 m_center;
    float m_radius;
    const Material* m_material;
    Aabb m_bound; // TODO SceneObject
  };
}
