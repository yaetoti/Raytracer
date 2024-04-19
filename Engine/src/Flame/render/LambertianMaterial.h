#pragma once

#include "IMaterial.h"
#include "Flame/math/MathUtils.h"

#include <random>

struct LambertianMaterial final : IMaterial {
  LambertianMaterial(const glm::vec3& albedo, std::uniform_real_distribution<float>& distribution, std::mt19937_64& generator)
  : m_albedo(albedo)
  , m_distribution(distribution)
  , m_generator(generator) {
  }

  bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, glm::vec3& attenuation) override {
    glm::vec3 target = record.point + record.normal + MathUtils::RandomUnitVector<3, float>(m_generator, m_distribution);
    scattered = Ray(record.point, glm::normalize(target - record.point));
    attenuation = m_albedo;
    return true;
  }

private:
  glm::vec3 m_albedo;
  std::uniform_real_distribution<float>& m_distribution;
  std::mt19937_64& m_generator;
};
