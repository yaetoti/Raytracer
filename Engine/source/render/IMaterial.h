#pragma once

#include "Engine/source/math/Vector.h"

struct IMaterial {
  virtual ~IMaterial() = default;

  virtual bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, Vec3F& attenuation) = 0;
};
