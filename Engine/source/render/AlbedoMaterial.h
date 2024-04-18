#pragma once
#include "IMaterial.h"

struct AlbedoMaterial final : IMaterial {
  AlbedoMaterial(const Vec3F& albedo)
  : m_albedo(albedo) {
  }

  bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, Vec3F& attenuation) override {
    scattered = Ray();
    attenuation = m_albedo;
    return true;
  }

private:
  Vec3F m_albedo;
};
