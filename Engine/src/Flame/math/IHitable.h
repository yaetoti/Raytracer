#pragma once

#include "HitRecord.h"
#include "Ray.h"

namespace Flame {
  struct IHitable {
    virtual ~IHitable() = default;

    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const = 0;
  };
}
