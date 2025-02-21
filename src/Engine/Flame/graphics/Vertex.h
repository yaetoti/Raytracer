#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace Flame {
  struct Vertex final {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 uv;
  };
}
