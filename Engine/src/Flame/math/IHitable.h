#pragma once

#include "HitRecord.h"
#include "Ray.h"

namespace Flame {
  struct IHitable {
    virtual ~IHitable() = default;

    virtual bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const = 0;
  };
}
