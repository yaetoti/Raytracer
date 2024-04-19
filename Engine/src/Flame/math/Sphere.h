#pragma once

#include "IHitable.h"
#include "Ray.h"
#include "Vec3.h"
#include <iostream>

struct IMaterial;

struct Sphere final : IHitable {
  Vec3<float> center;
  float radius;
  std::unique_ptr<IMaterial> material;

  explicit Sphere(const Vec3<float>& center, const float& radius, std::unique_ptr<IMaterial>&& material)
  : center(center)
  , radius(radius)
  , material(std::move(material)) {
  }

  bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) override {
    Vec3<float> oc = r.origin - center;
    float b = oc.Dot(r.direction);
    float c = oc.SquaredLength() - radius * radius;
    float d = b * b - c;

    if (d > 0) {
      float t = (-b - sqrt(d));
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - center) / radius;
        record.material = material.get();
        return true;
      }

      t = (-b + sqrt(d));
      if (t > tMin && t < tMax) {
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = (record.point - center) / radius;
        record.material = material.get();
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
