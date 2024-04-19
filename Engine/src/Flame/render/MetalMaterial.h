#pragma once

#include "IMaterial.h"

struct MetalMaterial final : IMaterial {
  explicit MetalMaterial(const Vec3F& albedo)
  : m_albedo(albedo) {
  }

  bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, Vec3F& attenuation) override {
    Vec3F reflected = ray.direction.Reflect(record.normal).Normalize();
    scattered = Ray(record.point, reflected);
    attenuation = m_albedo;
    return reflected.Dot(record.normal) > 0;
  }

private:
  Vec3F m_albedo;
};
