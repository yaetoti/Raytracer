#pragma once

#include "Flame/utils/SolidVector.h"
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

// struct PointLight final {
//   glm::vec3 position;
//   glm::vec3 color;
//   float intensity;

//   float linearFadeoff;
//   float constantFadeoff;
//   float quadraticFadeoff;
// };

// struct DirectLight final {
//   glm::vec3 direction;
//   glm::vec3 color;
//   float intensity;
// };

// struct SpotLight final {
//   glm::vec3 position;
//   glm::vec3 direction;
//   glm::vec3 color;
//   float intensity;
//   float cutoffCosineInner;
//   float cutoffCosineOuter;

//   float linearFadeoff;
//   float constantFadeoff;
//   float quadraticFadeoff;
// };

namespace Flame {
  struct SpotLight final {
    glm::vec3 position;
    glm::quat direction;
    glm::vec3 color;
    float intensity;
    // Texture mask
  };

  struct PointLight final {
    uint32_t parentTransformId;
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
  };

  struct DirectLight final {
    glm::vec3 color;
    glm::vec3 direction;
    float intensity;
  };

  struct LightSystem final {
    // Add...Light() : ID;
    // Get...Light(ID) : ...Light*;

    void Cleanup() {
      m_spotLights.clear();
      m_directLights.clear();
      m_pointLights.clear();
    }

    static LightSystem* Get() {
      static LightSystem instance;
      return &instance;
    }

  private:
    LightSystem() = default;

  private:
    SolidVector<SpotLight> m_spotLights;
    SolidVector<DirectLight> m_directLights;
    SolidVector<PointLight> m_pointLights;
  };
}
