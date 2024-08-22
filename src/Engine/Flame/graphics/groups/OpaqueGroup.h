#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Flame/engine/TransformSystem.h"
#include "ShaderGroup.h"
#include "Flame/engine/Transform.h"
#include "Flame/engine/Model.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"

namespace Flame {
  struct OpaqueInstanceData final {
    struct ShaderData final {
      glm::mat4 modelMatrix;
    };

    ShaderData GetShaderData() const {
      return ShaderData {
        TransformSystem::Get()->At(transformId)->transform.GetMat()
      };
    }

  public:
    uint32_t transformId;
  };

  struct OpaqueMaterialData final {
    // Empty for now
  };

  struct OpaqueGroup final : ShaderGroup<OpaqueInstanceData, OpaqueMaterialData> {
    void Init();
    void Cleanup();

    void InitInstanceBuffer();
    void UpdateInstanceBuffer();
    void Render();

  private:
    VertexBuffer<OpaqueInstanceData::ShaderData> m_instanceBuffer;
    bool m_instanceBufferDirty = true;

    VertexShader m_vertexShader;
    PixelShader m_pixelShader;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/opaque.hlsl";
    inline static const wchar_t* kFlashlightTexturePath = L"Assets/Textures/flashlight_1.dds";
  };
}
