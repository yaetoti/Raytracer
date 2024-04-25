#pragma once

#include "IMaterial.h"
#include "Flame/utils/Random.h"

namespace Flame {
  struct LambertianMaterial final : IMaterial {
    LambertianMaterial(const glm::vec3& albedo, float roughness)
    : m_albedo(albedo)
    , m_roughness(roughness) {
    }

    bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, glm::vec3& attenuation) const override {
      glm::vec3 reflected = glm::normalize(glm::reflect(ray.direction, record.normal + m_roughness * Random::UnitVector<3, float>()));
      scattered = Ray(record.point, reflected);
      attenuation = m_albedo;
      return true;
    }

  private:
    glm::vec3 m_albedo;
    float m_roughness;
  };
}
