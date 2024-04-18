#pragma once

#include "Vec3.h"

struct IMaterial;

struct HitRecord final {
  float time;
  Vec3<float> point;
  Vec3<float> normal;
  IMaterial* material;
};
