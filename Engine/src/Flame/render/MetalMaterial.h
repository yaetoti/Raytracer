#pragma once

#include "IMaterial.h"

struct MetalMaterial final : IMaterial {
  explicit MetalMaterial(const glm::vec3& albedo)
  : m_albedo(albedo) {
  }

  bool Scatter(const Ray& ray, const HitRecord& record, Ray& scattered, glm::vec3& attenuation) const override {
    glm::vec3 reflected = glm::normalize(glm::reflect(ray.direction, record.normal));
    scattered = Ray(record.point, reflected);
    attenuation = m_albedo;
    return glm::dot(reflected, record.normal) > 0;
  }

private:
  glm::vec3 m_albedo;
};
