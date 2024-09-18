#pragma once

#include <glm/glm.hpp>

namespace Flame {
  struct PerView final {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec4 frustumTL;
    glm::vec4 frustumBR;
    glm::vec4 frustumBL;
    glm::vec4 resolution;
    glm::vec4 cameraPosition;
  };
}
