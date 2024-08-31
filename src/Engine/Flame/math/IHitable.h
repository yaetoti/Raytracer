#pragma once

#include "HitRecordOld.h"
#include "Ray.h"

namespace Flame {
  struct IHitable {
    virtual ~IHitable() = default;

    virtual bool Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const = 0;
  };
}
