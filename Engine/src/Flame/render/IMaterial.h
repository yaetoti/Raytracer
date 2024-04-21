#pragma once

#include "Flame/math/Ray.h"

struct HitRecord;

struct IMaterial {
  virtual ~IMaterial() = default;

  virtual bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, glm::vec3& attenuation) const = 0;
};
