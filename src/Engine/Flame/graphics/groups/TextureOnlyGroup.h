#pragma once

#include "Flame/engine/TransformSystem.h"
#include "Flame/engine/Transform.h"
#include "Flame/graphics/groups/ShaderGroup.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include <d3d11.h>
#include <Flame/engine/ShaderPipeline.h>

namespace Flame {
  struct TextureOnlyInstanceData final {
    struct ShaderData final {
      glm::mat4 modelMatrix;
    };

    ShaderData GetShaderData() const {
      ShaderData data;
      data.modelMatrix = TransformSystem::Get()->At(transformId)->transform.GetMat();
      return data;
    }

  public:
    uint32_t transformId;
  };

  struct TextureOnlyMaterialData final {
    ID3D11ShaderResourceView* textureView;
  };

  struct TextureOnlyGroup final : ShaderGroup<TextureOnlyInstanceData, TextureOnlyMaterialData> {
    void Init();
    void Cleanup();

    void Render();

  private:
    void UpdateInstanceBufferData();
    void UpdateInstanceBuffer();

  private:
    ShaderPipeline m_pipeline;
    VertexBuffer<TextureOnlyInstanceData::ShaderData> m_instanceBuffer;
    uint32_t m_instanceCount = 0;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/texture_only.hlsl";
  };
}
