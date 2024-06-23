#include "Scene.h"

#include <execution>
#include <iostream>
#include <numeric>

#include "Flame/render/Camera.h"
#include "Flame/math/MathUtils.h"
#include "Flame/math/Ray.h"
#include "Flame/utils/Random.h"
#include "Flame/window/events/KeyWindowEvent.h"
#include "Flame/window/events/ResizeWindowEvent.h"

namespace Flame {
  void Scene::Render(Framebuffer& surface, const Camera& camera) {
    std::for_each(std::execution::par, m_rowIndices.begin(), m_rowIndices.end(), [&](uint32_t row) {
      std::for_each(std::execution::par, m_columnIndices.begin(), m_columnIndices.end(), [&](uint32_t col) {
        glm::vec3 light(0.0f);
        glm::vec3 color = ColorPerRay(camera, camera.GetRandomizedRay(col, row), 0, light);
        color = glm::clamp(color, glm::vec3(0), glm::vec3(1));

        // Accumulate color
        color.r = m_accumulatedData[(row * m_surfaceWidth + col) * 3 + 0] += color.r;
        color.g = m_accumulatedData[(row * m_surfaceWidth + col) * 3 + 1] += color.g;
        color.b = m_accumulatedData[(row * m_surfaceWidth + col) * 3 + 2] += color.b;
        color *= 255.0f / static_cast<float>(m_framesCount);

        surface.SetPixel(
          col, row,
          static_cast<BYTE>(color.r),
          static_cast<BYTE>(color.g),
          static_cast<BYTE>(color.b)
        );
      });
    });

    ++m_framesCount;
  }

  void Scene::HandleEvent(const WindowEvent& e) {
    if (e.type == WindowEventType::RESIZE) {
      auto evt = static_cast<const ResizeWindowEvent&>(e);
      m_surfaceWidth = evt.surfaceWidth;
      m_surfaceHeight = evt.surfaceHeight;
      m_columnIndices.resize(m_surfaceWidth);
      m_rowIndices.resize(m_surfaceHeight);
      std::iota(m_columnIndices.begin(), m_columnIndices.end(), 0);
      std::iota(m_rowIndices.begin(), m_rowIndices.end(), 0);

      m_accumulatedData.resize(m_surfaceWidth * m_surfaceHeight * 3);
      ResetAccumulatedData();

      return;
    }
  }

  void Scene::ResetAccumulatedData() {
    std::ranges::fill(m_accumulatedData, 0.0f);
    m_framesCount = 1;
  }

  std::vector<std::unique_ptr<IHitable>>& Scene::GetHitables() {
    return m_hitables;
  }

  glm::vec3 Scene::ColorPerRay(const Camera& camera, const Ray& ray, uint32_t bounce, glm::vec3& lightTotal) {
    HitRecord record;

    // Find surface for which the color will be calculated
    if (!MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), ray, 0.001f, std::numeric_limits<float>::max(), record)) {
      // No surface was hit
      glm::vec3 skyColorTop(0.066666f, 0.070588f, 0.180392f);
      glm::vec3 skyColorBottom(0.262745f, 0.207843f, 0.549019f);
      return glm::mix(skyColorTop, skyColorBottom, (ray.direction.y + 1) * 0.5f);
    }

    Material& material = *m_materials[record.materialId];
    // glm::vec3 color = record.material->albedo;
    glm::vec3 color = material.albedo;
    glm::vec3 colorReflected(0.0f);

    // Lighting
    glm::vec3 lightSurface = CalculatePointLightPerPoint(camera, record);
    lightSurface += CalculateSpotLightPerPoint(camera, record);
    lightSurface += CalculateDirectLightPerPoint(camera, record);
    lightSurface += material.emissionStrength * material.emissionColor;

    // TODO In Blender works differently
    // Calculate reflected color and light for metallic objects
    if (bounce < m_bounces) {
      // TODO perform for emission

      // Don't perform calculations for non-reflective materials
      // if (record.material->metallic > std::numeric_limits<float>::epsilon())
      {
        glm::vec3 normal = record.normal;
        // Don't perform calculations for smooth materials
        if (material.roughness > std::numeric_limits<float>::epsilon()) {
          normal += material.roughness * Random::UnitVector<3, float>();
        }

        Ray rayReflected(record.point, glm::normalize(glm::reflect(ray.direction, normal)));
        colorReflected = ColorPerRay(camera, rayReflected, bounce + 1, lightSurface);
      }
    } else {
      // I'm in too deep (Too much bounces)
      return glm::vec3(0.0f);
    }

    // TODO Global Illumination. Try #2 (success ?)
    lightTotal += lightSurface + material.emissionStrength * material.emissionColor;
    color *= lightTotal;

    // Mix colors
    color = glm::mix(color, colorReflected, material.metallic);

    return color;
  }

  glm::vec3 Scene::CalculatePointLightPerPoint(const Camera& camera, const HitRecord& record) {
    Material& material = *m_materials[record.materialId];
    glm::vec3 light(0.0f);

    for (auto& pointLight : m_pointLights) {
      glm::vec3 lightVec = pointLight->position - record.point;
      if (m_lightSmooth > std::numeric_limits<float>::epsilon()) {
        // Light vector is randomized to get different results
        lightVec += m_lightSmooth * Random::UnitVector<3, float, glm::packed_highp>();
      }

      float lightDistance = glm::length(lightVec);
      glm::vec3 lightDir = glm::normalize(lightVec);
      Ray rayLight(record.point + record.normal * 0.01f, lightDir);
      HitRecord recordLight;

      // Check for objects between point and light
      if (MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), rayLight, 0.0f, lightDistance, recordLight)) {
        // TODO Global Illumination. Try #1 (failed)
        // light += CalculatePointLightPerPoint(camera, recordLight, bounce + 1);
        // light += recordLight.material->emissionStrength * recordLight.material->emissionColor;
        continue;
      }

      // Attenuation
      float attenuation = 1.0f / (pointLight->constantFadeoff
                                  + pointLight->linearFadeoff * lightDistance
                                  + pointLight->quadraticFadeoff * lightDistance * lightDistance);
      // Diffuse
      glm::vec3 diffuse = material.diffuse * glm::max(glm::dot(record.normal, lightDir), 0.0f) * pointLight->color;
      // Specular
      glm::vec3 viewDir = glm::normalize(camera.GetPosition() - record.point);
      glm::vec3 halfReflect = glm::normalize(lightDir + viewDir);
      glm::vec3 specular = material.specular * glm::pow(glm::max(glm::dot(record.normal, halfReflect), 0.0f), material.specularExponent) * pointLight->color;

      // Accumulate light
      light += attenuation * (diffuse + specular) * pointLight->intensity;
    }

    return light;
  }

  glm::vec3 Scene::CalculateSpotLightPerPoint(const Camera& camera, const HitRecord& record) {
    Material& material = *m_materials[record.materialId];
    glm::vec3 light(0.0f);

    for (auto& spotLight : m_spotLights) {
      glm::vec3 lightVec = spotLight->position - record.point;
      if (m_lightSmooth > std::numeric_limits<float>::epsilon()) {
        // Light vector is randomized to get different results
        lightVec += m_lightSmooth * Random::UnitVector<3, float, glm::packed_highp>();
      }

      float lightDistance = glm::length(lightVec);
      glm::vec3 lightDir = glm::normalize(lightVec);

      float theta = glm::dot(lightDir, -spotLight->direction);
      float epsilon = spotLight->cutoffCosineInner - spotLight->cutoffCosineOuter;
      float intensity = glm::clamp((theta - spotLight->cutoffCosineOuter) / epsilon, 0.0f, 1.0f);
      if (intensity <= 0.001f) {
        return light;
      }

      Ray rayLight(record.point + record.normal * 0.01f, lightDir);
      HitRecord recordLight;

      // Check for objects between point and light
      if (MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), rayLight, 0.0f, lightDistance, recordLight)) {
        // TODO Global Illumination. Try #1 (failed)
        // light += CalculatePointLightPerPoint(camera, recordLight, bounce + 1);
        // light += recordLight.material->emissionStrength * recordLight.material->emissionColor;
        continue;
      }

      // Attenuation
      float attenuation = 1.0f / (spotLight->constantFadeoff
                                  + spotLight->linearFadeoff * lightDistance
                                  + spotLight->quadraticFadeoff * lightDistance * lightDistance);
      // Diffuse
      glm::vec3 diffuse = material.diffuse * glm::max(glm::dot(record.normal, lightDir), 0.0f) * spotLight->color;
      // Specular
      glm::vec3 viewDir = glm::normalize(camera.GetPosition() - record.point);
      glm::vec3 halfReflect = glm::normalize(lightDir + viewDir);
      glm::vec3 specular = material.specular * glm::pow(glm::max(glm::dot(record.normal, halfReflect), 0.0f), material.specularExponent) * spotLight->color;

      // Accumulate light
      light += attenuation * (diffuse + specular) * spotLight->intensity * intensity;
    }

    return light;
  }

  glm::vec3 Scene::CalculateDirectLightPerPoint(const Camera& camera, const HitRecord& record) {
    Material& material = *m_materials[record.materialId];
    glm::vec3 light(0.0f);

    for (auto& directLight : m_directLights) {
      // Diffuse
      glm::vec3 diffuse = material.diffuse * glm::max(glm::dot(record.normal, -directLight->direction), 0.0f) * directLight->color;
      // Specular
      glm::vec3 viewDir = glm::normalize(camera.GetPosition() - record.point);
      glm::vec3 halfReflect = glm::normalize(directLight->direction + viewDir);
      glm::vec3 specular = material.specular * glm::pow(glm::max(glm::dot(record.normal, halfReflect), 0.0f), material.specularExponent) * directLight->color;

      // Accumulate light
      light += (diffuse + specular) * directLight->intensity;
    }

    return light;
  }
}
