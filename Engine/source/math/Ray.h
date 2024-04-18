#pragma once

#include <ostream>

#include "Vec3.h"

struct Ray final {
  Vec3<float> origin;
  Vec3<float> direction;

  Ray()
  : origin(Vec3<float>())
  , direction(Vec3<float>()) {
  }

  explicit Ray(const Vec3<float>& origin, const Vec3<float>& direction)
  : origin(origin)
  , direction(direction) {
  }

  Vec3<float> AtParameter(float t) const {
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
