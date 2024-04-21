#pragma once

#include "IMaterial.h"
#include "Flame/utils/Random.h"

namespace Flame {
  struct LambertianMaterial final : IMaterial {
    LambertianMaterial(const glm::vec3& albedo)
    : m_albedo(albedo) {
    }

    bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, glm::vec3& attenuation) const override {
      glm::vec3 target = record.point + record.normal + Random::UnitVector<3, float>();
      scattered = Ray(record.point, glm::normalize(target - record.point));
      attenuation = m_albedo;
      return true;
    }

  private:
    glm::vec3 m_albedo;
  };
}
