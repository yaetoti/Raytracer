#include "Scene.h"

#include <execution>
#include <iostream>
#include <numeric>

#include "Flame/camera/Camera.h"
#include "Flame/math/MathUtils.h"
#include "Flame/math/Ray.h"
#include "Flame/utils/Random.h"
#include "Flame/window/events/KeyWindowEvent.h"
#include "Flame/window/events/ResizeWindowEvent.h"

namespace Flame {
  void Scene::Render(Framebuffer& surface, const Camera& camera) {
    std::for_each(std::execution::par, m_rowIndices.begin(), m_rowIndices.end(), [&](uint32_t row) {
      std::for_each(std::execution::par, m_columnIndices.begin(), m_columnIndices.end(), [&](uint32_t col) {
        glm::vec3 color(0.0f);
        // TODO replace with accumulation
        for (uint32_t sample = 0; sample < m_samples; ++sample) {
          color += ColorPerRay(camera, camera.GetRandomizedRay(col, row), 0);
        }

        color = glm::clamp(color * m_sampleCountInv, glm::vec3(0), glm::vec3(1));
        // color *= m_sampleCountInv;
        color *= 255.0f;
        surface.SetPixel(col, row, static_cast<BYTE>(color.r), static_cast<BYTE>(color.g), static_cast<BYTE>(color.b));
      });
    });
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
      return;
    }
  }

  std::vector<std::unique_ptr<IHitable>>& Scene::GetHitables() {
    return m_hitables;
  }

  glm::vec3 Scene::ColorPerRay(const Camera& camera, const Ray& ray, uint32_t bounce) {
    HitRecord record;

    // Find surface for which the color will be calculated
    if (!MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), ray, 0.001f, std::numeric_limits<float>::max(), record)) {
      // No surface was hit
      glm::vec3 skyColorTop(0.066666f, 0.070588f, 0.180392f);
      glm::vec3 skyColorBottom(0.262745f, 0.207843f, 0.549019f);
      return glm::mix(skyColorTop, skyColorBottom, (ray.direction.y + 1) * 0.5f);
    }

    glm::vec3 color = record.material->albedo + record.material->emissionStrength * record.material->emissionColor;
    // Lighting
    glm::vec3 light = LightPerPoint(camera, record, 0);
    // color *= glm::mix(light, glm::vec3(1.0f), record.material->metallic);
    color *= light;

    // TODO In Blender works differently
    // Calculate reflected light for metallic objects
    if (bounce < m_bounces) {
      // TODO perform for emission
      // Don't perform calculations for non-reflective materials
      // if (record.material->metallic > std::numeric_limits<float>::epsilon()) {
        glm::vec3 normal = record.normal;
        // Don't perform calculations for smooth materials
        if (record.material->roughness > std::numeric_limits<float>::epsilon()) {
          normal += record.material->roughness * Random::UnitVector<3, float>();
        }

        Ray rayReflected(record.point, glm::normalize(glm::reflect(ray.direction, normal)));
        glm::vec3 colorReflected = ColorPerRay(camera, rayReflected, bounce + 1);

        // Mix colors
        color = glm::mix(color, colorReflected, record.material->metallic);
        // color *= colorReflected;
      // }
    } else {
      // I'm in too deep (Too much bounces)
      return glm::vec3(0.0f);
    }

    // Lighting
    // color *= glm::mix(light, glm::vec3(1.0f), record.material->metallic);
    // color *= light;

    return color;
  }

  glm::vec3 Scene::LightPerPoint(const Camera& camera, const HitRecord& record, uint32_t bounce) {
    //if (bounce > m_lightBounces) {
    //  return glm::vec3(0.0f);
    //}

    glm::vec3 light(0.0f);

    for (uint32_t lightSample = 0; lightSample < m_lightSamples; ++lightSample) {
      for (const PointLight& pointLight : m_pointLights) {
        glm::vec3 lightVec = pointLight.position - record.point;
        if (m_lightSmooth > std::numeric_limits<float>::epsilon()) {
          // Light vector is randomized to get different results
          lightVec += m_lightSmooth * Random::UnitVector<3, float, glm::packed_highp>();
        }

        glm::vec3 lightDir = glm::normalize(lightVec);

        Ray rayLight(record.point + record.normal * 0.01f, lightDir);
        HitRecord recordLight;
        // Check for objects between point and light
        if (MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), rayLight, 0.0f, glm::length(lightVec), recordLight)) {
          // light += LightPerPoint(camera, recordLight, bounce + 1);
          continue;
        }

        // Diffuse
        glm::vec3 diffuse = record.material->diffuse * glm::max(glm::dot(record.normal, lightDir), 0.0f) * pointLight.color;
        // Specular
        glm::vec3 viewDir = glm::normalize(camera.GetPosition() - record.point);
        glm::vec3 halfReflect = glm::normalize(lightDir + viewDir);
        glm::vec3 specular = record.material->specular * glm::pow(glm::max(glm::dot(record.normal, halfReflect), 0.0f), record.material->specularExponent) * pointLight.color;

        // Accumulate light
        light += (diffuse + specular) * pointLight.intensity;
      }
    }

    light *= 1.0f / m_lightSamples;
    return light;
  }
}
