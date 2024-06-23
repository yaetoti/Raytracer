#pragma once

#include "Aabb.h"
#include "HitRecord.h"
#include "Ray.h"
#include "Sphere.h"

namespace Flame {
  struct MathUtils {
    template <typename It>
    static bool HitClosest(It begin, It end, const Ray& r, float tMin, float tMax, HitRecord& record) {
      HitRecord tempRecord;
      bool hitAnything = false;
      float closest = tMax;

      while (begin != end) {
        if ((*begin)->Hit(r, tempRecord, tMin, closest)) {
          hitAnything = true;
          closest = tempRecord.time;
          record = tempRecord;
        }
        ++begin;
      }

      return hitAnything;
    }

    static glm::vec3 ColorFromHex(uint32_t color) {
      return glm::vec3(
        static_cast<float>(color >> 16 & 0xFF) / 255.0f,
        static_cast<float>(color >> 8 & 0xFF) / 255.0f,
        static_cast<float>(color & 0xFF)  / 255.0f
      );
    }

    static Aabb AabbFromSphere(const Sphere& sphere) {
      glm::vec3 min;
      glm::vec3 max;

      for (int dim = 0; dim < 3; ++dim) {
        min[dim] = sphere.Center()[dim] - sphere.Radius();
        max[dim] = sphere.Center()[dim] + sphere.Radius();
      }

      return Aabb(min, max);
    }
  };
}
