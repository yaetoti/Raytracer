#pragma once

#include "IHitable.h"
#include "Ray.h"
#include <iostream>

struct IMaterial;

struct Sphere final : IHitable {
  glm::vec3 center;
  float radius;
  const IMaterial& material;

  explicit Sphere(const glm::vec3& center, const float& radius, const IMaterial& material)
  : center(center)
  , radius(radius)
  , material(std::move(material)) {
  }

  bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) override {
    glm::vec3 oc = r.origin - center;

    float b = glm::dot(oc, r.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float d = b * b - c;

    if (d > 0) {
      float t = -b - sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - center) / radius;
        record.material = &material;
        return true;
      }

      t = -b + sqrtf(d);
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - center) / radius;
        record.material = &material;
        return true;
      }
    }

    return false;
  }

  friend std::ostream& operator<<(std::ostream& out, const Sphere& s) {
    out << "{ C: " << s.center << ", R: " << s.radius << " }";
    return out;
  }

  friend std::wostream& operator<<(std::wostream& out, const Sphere& s) {
    out << L"{ C: " << s.center << L", R: " << s.radius << L" }";
    return out;
  }
};
