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
    static constexpr uint32_t kDirectLightNum = 1;
    static constexpr uint32_t kPointLightNum = 8;
    static constexpr uint32_t kSpotLightNum = 1;

  public:
    struct ShaderData final {
      DirectLight::ShaderData directLightData[kDirectLightNum];
      PointLight::ShaderData pointLightData[kPointLightNum];
      SpotLight::ShaderData spotLightData[kSpotLightNum];
      uint32_t directLightCount;
      uint32_t pointLightCount;
      uint32_t spotLightCount;
      float padding0;
    };

    void Init();
    void Cleanup();

    uint32_t AddDirectLight(std::shared_ptr<DirectLight> light);
    uint32_t AddPointLight(std::shared_ptr<PointLight> light);
    uint32_t AddSpotLight(std::shared_ptr<SpotLight> light);
    void RemoveDirectLight(uint32_t id);
    void RemovePointLight(uint32_t id);
    void RemoveSpotLight(uint32_t id);
    std::shared_ptr<DirectLight> GetDirectLight(uint32_t id);
    std::shared_ptr<PointLight> GetPointLight(uint32_t id);
    std::shared_ptr<SpotLight> GetSpotLight(uint32_t id);
    SolidVector<std::shared_ptr<DirectLight>>& GetDirectLights();
    SolidVector<std::shared_ptr<PointLight>>& GetPointLights();
    SolidVector<std::shared_ptr<SpotLight>>& GetSpotLights();

    ShaderData ToShaderData() const;
    void CommitChanges();
    ID3D11Buffer* GetConstantBuffer() const;

    static LightSystem* Get();

  private:
    LightSystem() = default;

  private:
    SolidVector<std::shared_ptr<DirectLight>> m_directLights;
    SolidVector<std::shared_ptr<PointLight>> m_pointLights;
    SolidVector<std::shared_ptr<SpotLight>> m_spotLights;

    ConstantBuffer<ShaderData> m_constantBuffer;
  };
}
