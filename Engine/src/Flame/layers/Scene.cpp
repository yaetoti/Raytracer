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
    glm::vec3 color(0.0f);

    if (!MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), ray, 0.0001f, std::numeric_limits<float>::max(), record)) {
      // No surface was hit
      glm::vec3 skyColorTop(0.066666f, 0.070588f, 0.180392f);
      glm::vec3 skyColorBottom(0.262745f, 0.207843f, 0.549019f);
      return glm::mix(skyColorTop, skyColorBottom, (ray.direction.y + 1) * 0.5f);
    }

    color += record.material->albedo;

    //do {
    //  
    //} while(record.mat.metallic > std::numeric_limits<float>::epsilon() && bounce++ < m_bounces);

    //for (uint32_t bounce = 0; bounce < m_bounces; ++bounce) {
    //  // Find surface for which the color will be calculated
    //  if (!MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), ray, 0.0001f, std::numeric_limits<float>::max(), record)) {
    //    // No surface was hit
    //    glm::vec3 skyColorTop(0.066666f, 0.070588f, 0.180392f);
    //    glm::vec3 skyColorBottom(0.262745f, 0.207843f, 0.549019f);
    //    color += glm::mix(skyColorTop, skyColorBottom, (ray.direction.y + 1) * 0.5f);
    //    break;
    //  }

    //  ray.origin = record.point;
    //  ray.direction = glm::normalize(glm::reflect(ray.direction, record.normal + record.mat.roughness * Random::UnitVector<3, float>()));
    //}

    return color;



    // TODO remove
#if 0

    // Lighting
    // glm::vec3 lightColor(1.0f, 0.784313, 0.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(5.0, 6.0, -3.0);
    float ambientStrength = 0.55f;
    float diffuseStrength = 4.0f;
    float specularStrength = 1.0f;
    float specularExponent = 32;

    // Find surface for which the color will be calculated
    HitRecord record;
    if (MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), ray, 0.01f, std::numeric_limits<float>::max(), record)) {
      Ray scattered;
      glm::vec3 attenuation;

      // Recursively bounce N times
      // Scatter a ray 
      if (bounces < m_bounces && record.material->Scatter(ray, record, scattered, attenuation)) {
        // TODO material handling
        // Get color for reflected ray TODO and refracted
        glm::vec3 color = attenuation * ColorPerRay(camera, scattered, bounces + 1);
        glm::vec3 sampledLighting(0);

        glm::vec3 ambient = ambientStrength * lightColor;

        // For each light source
        for (uint32_t lightSample = 0; lightSample < m_lightSamples; ++lightSample) {
          // Diffuse
          // Light vector is randomized to get different results
          glm::vec3 lightVec = lightPos + m_lightSmooth * Random::UnitVector<3, float, glm::packed_highp>() - record.point;
          glm::vec3 lightDir = glm::normalize(lightVec);

          Ray ray2(record.point + record.normal * 0.01, lightDir);
          HitRecord record2;

          // Check for objects between point and light
          if (MathUtils::HitClosest(m_hitables.begin(), m_hitables.end(), ray2, 0, glm::length(lightVec), record2)) {
            // No light was hit
            return glm::clamp(color * ambient, glm::vec3(0.0f), glm::vec3(1.0f));
          }

          // Hit light

          glm::vec3 diffuse = diffuseStrength * glm::max(glm::dot(record.normal, lightDir), 0.0f) * lightColor;
          // Specular
          glm::vec3 viewDir = glm::normalize(camera.GetPosition() - record.point);
          glm::vec3 halfReflect = glm::normalize(lightDir + viewDir);
          glm::vec3 specular = specularStrength * glm::pow(glm::max(glm::dot(record.normal, halfReflect), 0.0f), specularExponent) * lightColor;

          sampledLighting += ambient + diffuse + specular;
        }

        // Average color among all light samples
        sampledLighting /= m_lightSamples;
        return glm::clamp(color * sampledLighting, glm::vec3(0), glm::vec3(1));
      }

      // Too much bounces
      return glm::vec3(0.0f);
    }

    // glm::vec3 skyColor(0.5f, 0.7f, 1.0f);
    glm::vec3 skyColor(0.066666f, 0.070588f, 0.180392f);
    glm::vec3 skyColor2(0.262745f, 0.207843f, 0.549019f);
    float t = (ray.direction.y + 1) * 0.5f;
    return skyColor * t + (1.0f - t) * skyColor2;
#endif
  }
}
