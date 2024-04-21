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
        if ((*begin)->Hit(r, tMin, closest, tempRecord)) {
          hitAnything = true;
          closest = tempRecord.time;
          record = tempRecord;
        }
        ++begin;
      }

      return hitAnything;
    }
  };
}
