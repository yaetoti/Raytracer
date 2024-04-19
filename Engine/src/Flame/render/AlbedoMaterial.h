#pragma once

#include "IMaterial.h"

struct AlbedoMaterial final : IMaterial {
  explicit AlbedoMaterial(const glm::vec3& albedo)
  : m_albedo(albedo) {
  }

  bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, glm::vec3& attenuation) override {
    scattered = Ray();
    attenuation = m_albedo;
    return true;
  }

private:
  glm::vec3 m_albedo;
};
