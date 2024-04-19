#pragma once

#include <glm/glm.hpp>

struct IMaterial;

struct HitRecord final {
  float time;
  glm::vec3 point;
  glm::vec3 normal;
  IMaterial* material;
};
