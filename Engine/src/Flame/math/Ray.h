#pragma once

#include <ostream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

struct Ray final {
  glm::vec3 origin;
  glm::vec3 direction;

  Ray()
  : origin(glm::vec3())
  , direction(glm::vec3()) {
  }

  explicit Ray(const glm::vec3& origin, const glm::vec3& direction)
  : origin(origin)
  , direction(direction) {
  }

  glm::vec3 AtParameter(float t) const {
    return origin + t * direction;
  }

  friend std::ostream& operator<<(std::ostream& out, const Ray ray) {
    out << "{ C: " << ray.origin << ", D: " << ray.direction << " }";
    return out;
  }

  friend std::wostream& operator<<(std::wostream& out, const Ray ray) {
    out << L"{ C: " << ray.origin << L", D: " << ray.direction << L" }";
    return out;
  }
};
