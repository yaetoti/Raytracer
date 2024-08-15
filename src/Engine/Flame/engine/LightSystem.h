#pragma once

#include "Flame/graphics/buffers/ConstantBuffer.h"
#include "Flame/utils/SolidVector.h"
#include "lights/DirectLight.h"
#include "lights/PointLight.h"
#include "lights/SpotLight.h"
#include <cstdint>
#include <glm/vec3.hpp>
#include <memory>

namespace Flame {
  struct LightSystem final {
  private:
    inline static constexpr uint32_t kLightNum = 8;

  public:
    struct ShaderData final {
      DirectLight::ShaderData directLightData[kLightNum];
      PointLight::ShaderData pointLightData[kLightNum];
      SpotLight::ShaderData spotLightData[kLightNum];
      uint32_t directLightCount;
      uint32_t pointLightCount;
      uint32_t spotLightCount;
      float padding0;
    };

    uint32_t AddDirectLight(std::shared_ptr<DirectLight> light) {
      return m_directLights.emplace(std::move(light));
    }

    uint32_t AddPointLight(std::shared_ptr<PointLight> light) {
      return m_pointLights.emplace(std::move(light));
    }

    uint32_t AddSpotLight(std::shared_ptr<SpotLight> light) {
      return m_spotLights.emplace(std::move(light));
    }

    void RemoveDirectLight(uint32_t id) {
      m_directLights.erase(id);
    }

    void RemovePointLight(uint32_t id) {
      m_pointLights.erase(id);
    }

    void RemoveSpotLight(uint32_t id) {
      m_spotLights.erase(id);
    }

    std::shared_ptr<DirectLight> GetDirectLight(uint32_t id) {
      return m_directLights[id];
    }

    std::shared_ptr<PointLight> GetPointLight(uint32_t id) {
      return m_pointLights[id];
    }

    std::shared_ptr<SpotLight> GetSpotLight(uint32_t id) {
      return m_spotLights[id];
    }

    ShaderData ToShaderData() const {
      ShaderData data;
      data.directLightCount = std::min(m_directLights.size(), kLightNum);
      data.pointLightCount = std::min(m_pointLights.size(), kLightNum);
      data.spotLightCount = std::min(m_spotLights.size(), kLightNum);

      for (uint32_t i = 0; i < data.directLightCount; ++i) {
        data.directLightData[i] = m_directLights[i]->ToShaderData();
      }

      for (uint32_t i = 0; i < data.pointLightCount; ++i) {
        data.pointLightData[i] = m_pointLights[i]->ToShaderData();
      }

      for (uint32_t i = 0; i < data.spotLightCount; ++i) {
        data.spotLightData[i] = m_spotLights[i]->ToShaderData();
      }

      return data;
    }

    void Init() {
      m_constantBuffer.Init();
    }

    void Cleanup() {
      m_spotLights.clear();
      m_directLights.clear();
      m_pointLights.clear();
    }

    void UpdateConstantBuffer() {
      m_constantBuffer.data = ToShaderData();
      m_constantBuffer.ApplyChanges();
    }

    ID3D11Buffer* GetConstantBuffer() const {
      return m_constantBuffer.Get();
    }

    static LightSystem* Get() {
      static LightSystem instance;
      return &instance;
    }

  private:
    LightSystem() = default;

  private:
    SolidVector<std::shared_ptr<DirectLight>> m_directLights;
    SolidVector<std::shared_ptr<PointLight>> m_pointLights;
    SolidVector<std::shared_ptr<SpotLight>> m_spotLights;

    ConstantBuffer<ShaderData> m_constantBuffer;
  };
}
