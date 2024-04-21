#pragma once

#include "Flame/render/IMaterial.h"
#include <glm/glm.hpp>

struct HitRecord final {
  float time;
  glm::vec3 point;
  glm::vec3 normal;
  const IMaterial* material;
};
