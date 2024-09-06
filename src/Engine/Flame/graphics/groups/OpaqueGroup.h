#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>
#include <Flame/engine/ShaderPipeline.h>
#include <glm/glm.hpp>

#include "Flame/engine/TransformSystem.h"
#include "Flame/graphics/buffers/ConstantBuffer.h"
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
    ID3D11ShaderResourceView* m_albedoView;
    ID3D11ShaderResourceView* m_normalView;
    ID3D11ShaderResourceView* m_roughnessView;
    ID3D11ShaderResourceView* m_metallicView;
  };

  struct OpaqueMeshData final {
    glm::mat4 meshToModel;
    glm::mat4 modelToMesh;
  };

  struct OpaqueGroup final : ShaderGroup<OpaqueInstanceData, OpaqueMaterialData> {
    void Init();
    void Cleanup();

    void InitInstanceBuffer();
    void UpdateInstanceBuffer();
    void Render();

  private:
    ShaderPipeline m_pipeline;
    VertexBuffer<OpaqueInstanceData::ShaderData> m_instanceBuffer;
    ConstantBuffer<OpaqueMeshData> m_meshMatrixBuffer;
    bool m_instanceBufferDirty = true;

    // IBL
    ID3D11ShaderResourceView* m_diffuseView = nullptr;
    ID3D11ShaderResourceView* m_specularView = nullptr;
    ID3D11ShaderResourceView* m_reflectanceView = nullptr;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/opaque.hlsl";
    inline static const wchar_t* kFlashlightTexturePath = L"Assets/Textures/flashlight_1.dds";
  };
}
