#pragma once
#include "IMaterial.h"

struct LambertianMaterial final : IMaterial {
  LambertianMaterial(const Vec3F& albedo, std::uniform_real_distribution<float>& distribution, std::mt19937& generator)
  : m_albedo(albedo)
  , m_distribution(distribution)
  , m_generator(generator) {
  }

  bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, Vec3F& attenuation) override {
    Vec3<float> target = record.point + record.normal + Vec3<float>::RandomInUnitSphere(m_distribution, m_generator);
    scattered = Ray(record.point, (target - record.point).Normalize());
    attenuation = m_albedo;
    return true;
  }

private:
  Vec3F m_albedo;
  std::uniform_real_distribution<float>& m_distribution;
  std::mt19937& m_generator;
};
