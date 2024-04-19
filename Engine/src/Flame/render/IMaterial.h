#pragma once

#include "Flame/math/HitRecord.h"
#include "Flame/math/Ray.h"

struct IMaterial {
  virtual ~IMaterial() = default;

  virtual bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, glm::vec3& attenuation) = 0;
};
