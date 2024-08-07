#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

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
      data.modelMatrix = transform.GetMat();
      data.mainColor = mainColor;
      data.secondaryColor = secondaryColor;
      return data;
    }

  public:
    Transform transform;
    glm::vec3 mainColor;
    glm::vec3 secondaryColor;
  };

  struct HologramMaterialData final {
    // Empty for now
  };

  struct HologramGroup final : ShaderGroup<HologramInstanceData, HologramMaterialData> {
    void Init();
    void Cleanup();

    bool HitInstance(const Ray& ray, HitRecord<PerInstance*>& record, float tMin, float tMax) const;
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
