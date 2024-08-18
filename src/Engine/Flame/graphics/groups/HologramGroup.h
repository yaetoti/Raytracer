#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Flame/engine/TransformSystem.h"
#include "Flame/utils/SolidVector.h"
#include "ShaderGroup.h"
#include "Flame/engine/Transform.h"
#include "Flame/engine/Model.h"
#include "Flame/graphics/buffers/VertexBuffer.h"
#include "Flame/graphics/shaders/GeometryShader.h"
#include "Flame/graphics/shaders/HullShader.h"
#include "Flame/graphics/shaders/DomainShader.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/math/HitRecord.h"

namespace Flame {
  struct HologramInstanceData final {
    struct ShaderData final {
      glm::mat4 modelMatrix;
      glm::vec3 mainColor;
      glm::vec3 secondaryColor;
    };

    struct ShaderNormalData final {
      
    };

    ShaderData GetShaderData() const {
      ShaderData data;
      data.modelMatrix = TransformSystem::Get()->At(transformId)->transform.GetMat();
      data.mainColor = mainColor;
      data.secondaryColor = secondaryColor;
      return data;
    }

  public:
    uint32_t transformId;
    glm::vec3 mainColor;
    glm::vec3 secondaryColor;
  };

  struct HologramMaterialData final {
    // Empty for now
  };

  struct HologramGroup final : ShaderGroup<HologramInstanceData, HologramMaterialData> {
    void Init();
    void Cleanup();

    void UpdateInstanceBuffer();
    void Render();

  private:
    VertexBuffer<HologramInstanceData::ShaderData> m_instanceBuffer;
    bool m_instanceBufferDirty = true;

    VertexShader m_vertexShader;
    PixelShader m_pixelShader;
    GeometryShader m_geometryShader;
    HullShader m_hullShader;
    DomainShader m_domainShader;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/hologram.hlsl";
  };
}
