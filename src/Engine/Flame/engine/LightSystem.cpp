#include "LightSystem.h"

namespace Flame {
  void LightSystem::Init() {
    m_constantBuffer.Init();
  }

  void LightSystem::Cleanup() {
    m_spotLights.clear();
    m_directLights.clear();
    m_pointLights.clear();
    m_constantBuffer.Reset();
  }

  uint32_t LightSystem::AddDirectLight(std::shared_ptr<DirectLight> light) {
    return m_directLights.emplace(std::move(light));
  }

  uint32_t LightSystem::AddPointLight(std::shared_ptr<PointLight> light) {
    return m_pointLights.emplace(std::move(light));
  }

  uint32_t LightSystem::AddSpotLight(std::shared_ptr<SpotLight> light) {
    return m_spotLights.emplace(std::move(light));
  }

  void LightSystem::RemoveDirectLight(uint32_t id) {
    m_directLights.erase(id);
  }

  void LightSystem::RemovePointLight(uint32_t id) {
    m_pointLights.erase(id);
  }

  void LightSystem::RemoveSpotLight(uint32_t id) {
    m_spotLights.erase(id);
  }

  std::shared_ptr<DirectLight> LightSystem::GetDirectLight(uint32_t id) {
    return m_directLights[id];
  }

  std::shared_ptr<PointLight> LightSystem::GetPointLight(uint32_t id) {
    return m_pointLights[id];
  }

  std::shared_ptr<SpotLight> LightSystem::GetSpotLight(uint32_t id) {
    return m_spotLights[id];
  }

  SolidVector<std::shared_ptr<DirectLight>>& LightSystem::GetDirectLights() {
    return m_directLights;
  }

  SolidVector<std::shared_ptr<PointLight>>& LightSystem::GetPointLights() {
    return m_pointLights;
  }

  SolidVector<std::shared_ptr<SpotLight>>& LightSystem::GetSpotLights() {
    return m_spotLights;
  }

  LightSystem::ShaderData LightSystem::ToShaderData() const {
    ShaderData data;
    data.directLightCount = std::min(m_directLights.size(), kDirectLightNum);
    data.pointLightCount = std::min(m_pointLights.size(), kPointLightNum);
    data.spotLightCount = std::min(m_spotLights.size(), kSpotLightNum);

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

  void LightSystem::CommitChanges() {
    m_constantBuffer.data = ToShaderData();
    m_constantBuffer.ApplyChanges();
  }

  ID3D11Buffer* LightSystem::GetConstantBuffer() const {
    return m_constantBuffer.Get();
  }

  LightSystem* LightSystem::Get() {
    static LightSystem instance;
    return &instance;
  }
}
