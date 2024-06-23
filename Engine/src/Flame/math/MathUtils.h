#pragma once

#include "Aabb.h"
#include "HitRecord.h"
#include "Ray.h"
#include "Sphere.h"
#include "Triangle.h"

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
        min[dim] = sphere.center[dim] - sphere.radius;
        max[dim] = sphere.center[dim] + sphere.radius;
      }

      return Aabb(min, max);
    }

    static Aabb AabbFromTriangle(const Triangle& triangle) {
      glm::vec3 min(std::numeric_limits<float>::infinity());
      glm::vec3 max(-std::numeric_limits<float>::infinity());
      for (int point = 0; point < 3; ++point) {
        for (int dim = 0; dim < 3; ++dim) {
          min[dim] = glm::min(triangle.points[point][dim], min[dim]);
          max[dim] = glm::max(triangle.points[point][dim], max[dim]);
        }
      }

      return Aabb(min, max);
    }
  };
}
