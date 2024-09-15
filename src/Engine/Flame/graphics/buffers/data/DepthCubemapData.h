#pragma once
#include <cstdint>

#include "glm/vec3.hpp"

struct DepthCubemapData final {
  glm::vec3 position;
  uint32_t cubemapIndex;
};
