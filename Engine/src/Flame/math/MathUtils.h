#pragma once

#include "HitRecord.h"
#include "Ray.h"

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

    template <typename It>
    static bool HitClosest(It begin, It end, const Ray& r, float tMin, float tMax, HitRecord& record, bool ignoreDebug) {
      HitRecord tempRecord;
      bool hitAnything = false;
      float closest = tMax;

      while (begin != end) {
        if ((*begin++)->Hit(r, tempRecord, tMin, closest)) {
          // TODO: Temporary solution for light visualization. Remove or refactor later
          if (tempRecord.material->debugMaterial && ignoreDebug) {
            continue;
          }

          hitAnything = true;
          closest = tempRecord.time;
          record = tempRecord;
        }
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
  };
}
