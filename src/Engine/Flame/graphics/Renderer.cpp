#include "Renderer.h"

#include <algorithm>
#include <execution>

#include "Flame/math/MathUtils.h"
#include "Flame/utils/Random.h"

namespace Flame {
  Renderer::Renderer(const SceneOld* scene)
  : m_scene(scene)
  , m_executor(std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 1)) {
  }

  void Renderer::Render(Framebuffer& surface, const AlignedCamera& camera) {
    //glm::vec3 light;
    //std::cout << ColorPerRay(camera, camera.GetRandomizedRay(20, 20), 0, light) << '\n';
    //std::cout << "Light:" << light << '\n';

    m_executor.Execute([this, &surface, &camera](uint32_t threadIndex, uint32_t taskIndex) {
      uint32_t row = taskIndex / m_surfaceWidth;
      uint32_t col = taskIndex % m_surfaceWidth;

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
    }, m_surfaceWidth * m_surfaceHeight, 20);

    ++m_framesCount;
  }

  void Renderer::Resize(uint32_t width, uint32_t height) {
    m_surfaceWidth = width;
    m_surfaceHeight = height;
    m_accumulatedData.resize(m_surfaceWidth * m_surfaceHeight * 3);
    ResetAccumulatedData();
  }

  void Renderer::ResetAccumulatedData() {
    std::ranges::fill(m_accumulatedData, 0.0f);
    m_framesCount = 1;
  }

  glm::vec3 Renderer::ColorPerRay(const AlignedCamera& camera, const Ray& ray, uint32_t bounce, glm::vec3& lightTotal) {
    const auto& hitables = m_scene->GetHitables();
    const auto& materials = m_scene->GetMaterials();
    HitRecordOld record;

    // Find surface for which the color will be calculated
    if (!MathUtils::HitClosest(hitables.begin(), hitables.end(), ray, 0.001f, std::numeric_limits<float>::max(), record)) {
      // No surface was hit
      glm::vec3 skyColorTop(0.066666f, 0.070588f, 0.180392f);
      glm::vec3 skyColorBottom(0.262745f, 0.207843f, 0.549019f);
      return glm::mix(skyColorTop, skyColorBottom, (ray.direction.y + 1) * 0.5f);
    }

    Material& material = *materials[record.materialId];
    glm::vec3 color = material.albedo;
    glm::vec3 colorReflected(0.0f);

    // Lighting
    glm::vec3 lightSurface = glm::vec3(0.0f);
    lightSurface += CalculatePointLightPerPoint(camera, record);
    lightSurface += CalculateSpotLightPerPoint(camera, record);
    lightSurface += CalculateDirectLightPerPoint(camera, record);
    lightSurface += material.emissionStrength * material.emissionColor;
    // Quick reflection fix. Don't know how it should actually work
    // Objects in the mirror are brighter than in reality
    // That's because we calculate light in 100% cases for the surface on the contrary to reflected rays. Not enough samples (?)
    lightSurface *= 1.0f - material.metallic;


    // TODO In Blender works differently
    // Calculate reflected color and light for metallic objects
    if (bounce < m_bounces) {
      // TODO perform for emission

      // Don't perform calculations for non-reflective materials
      if (material.metallic > std::numeric_limits<float>::epsilon())
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

    lightTotal += lightSurface + material.emissionStrength * material.emissionColor;
    color *= lightTotal;

    // Mix colors
    color = glm::mix(color, colorReflected, material.metallic);

    return color;
  }

  glm::vec3 Renderer::CalculatePointLightPerPoint(const AlignedCamera& camera, const HitRecordOld& record) {
    const auto& hitables = m_scene->GetHitables();
    const auto& materials = m_scene->GetMaterials();
    const auto& pointLights = m_scene->GetPointLights();
    Material& material = *materials[record.materialId];
    glm::vec3 light(0.0f);

    for (auto& pointLight : pointLights) {
      glm::vec3 lightVec = pointLight->position - record.point;
      if (m_lightSmooth > std::numeric_limits<float>::epsilon()) {
        // Light vector is randomized to get different results
        lightVec += m_lightSmooth * Random::UnitVector<3, float, glm::packed_highp>();
      }

      float lightDistance = glm::length(lightVec);
      glm::vec3 lightDir = glm::normalize(lightVec);
      Ray rayLight(record.point + record.normal * 0.01f, lightDir);
      HitRecordOld recordLight;

      // Check for objects between point and light
      if (MathUtils::HitClosest(hitables.begin(), hitables.end(), rayLight, 0.0f, lightDistance, recordLight)) {
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

  glm::vec3 Renderer::CalculateSpotLightPerPoint(const AlignedCamera& camera, const HitRecordOld& record) {
    const auto& hitables = m_scene->GetHitables();
    const auto& materials = m_scene->GetMaterials();
    const auto& spotLights = m_scene->GetSpotLights();
    Material& material = *materials[record.materialId];
    glm::vec3 light(0.0f);

    for (auto& spotLight : spotLights) {
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
      HitRecordOld recordLight;

      // Check for objects between point and light
      if (MathUtils::HitClosest(hitables.begin(), hitables.end(), rayLight, 0.0f, lightDistance, recordLight)) {
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

  glm::vec3 Renderer::CalculateDirectLightPerPoint(const AlignedCamera& camera, const HitRecordOld& record) {
    const auto& materials = m_scene->GetMaterials();
    const auto& directLights = m_scene->GetDirectLights();
    Material& material = *materials[record.materialId];
    glm::vec3 light(0.0f);

    for (auto& directLight : directLights) {
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
