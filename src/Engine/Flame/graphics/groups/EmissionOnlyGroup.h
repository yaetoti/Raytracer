#pragma once
#include <memory>
#include <vector>
#include <Flame/engine/ShaderPipeline.h>
#include <glm/glm.hpp>

#include "Flame/engine/TransformSystem.h"
#include "ShaderGroup.h"
#include "Flame/engine/Transform.h"
#include "Flame/engine/Model.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"

namespace Flame {
  struct EmissionOnlyInstanceData final {
    struct ShaderData final {
      glm::mat4 modelMatrix;
      glm::vec3 emission;
    };

    ShaderData GetShaderData() const {
      ShaderData data;
      data.modelMatrix = TransformSystem::Get()->At(transformId)->transform.GetMat();
      data.emission = emission;
      return data;
    }

  public:
    uint32_t transformId;
    glm::vec3 emission;
  };

  struct EmissionOnlyMaterialData final {
    // Empty for now
  };

  struct EmissionOnlyGroup final : ShaderGroup<EmissionOnlyInstanceData, EmissionOnlyMaterialData> {
    void Init();
    void Cleanup();

    void Render();

  private:
    void UpdateInstanceBufferData();
    void UpdateInstanceBuffer();

  private:
    ShaderPipeline m_pipeline;
    VertexBuffer<EmissionOnlyInstanceData::ShaderData> m_instanceBuffer;
    uint32_t m_instanceCount = 0;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/emission.hlsl";
  };
}
