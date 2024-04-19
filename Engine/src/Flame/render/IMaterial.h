#pragma once

#include "Flame/math/HitRecord.h"
#include "Flame/math/Ray.h"
#include "Flame/math/Vector.h"

struct IMaterial {
  virtual ~IMaterial() = default;

  virtual bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, Vec3F& attenuation) = 0;
};
