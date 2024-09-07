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

    struct Depth2DShaderData final {
      glm::mat4 modelMatrix;
    };

    ShaderData GetShaderData() const {
      return ShaderData {
        TransformSystem::Get()->At(transformId)->transform.GetMat()
      };
    }

    Depth2DShaderData GetDepth2DShaderData() const {
      return Depth2DShaderData {
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

    void Render();
    void RenderDepth2D();
    void RenderDepthCubemaps(std::span<glm::vec3> positions);

  private:
    void UpdateInstanceBufferData();
    void UpdateInstanceBuffer();

  private:
    ShaderPipeline m_pipeline;
    ShaderPipeline m_depth2DPipeline;
    VertexBuffer<OpaqueInstanceData::ShaderData> m_instanceBuffer;
    VertexBuffer<OpaqueInstanceData::Depth2DShaderData> m_depth2DInstanceBuffer;
    uint32_t m_instanceCount = 0;
    ConstantBuffer<OpaqueMeshData> m_meshBuffer;

    // IBL
    ID3D11ShaderResourceView* m_diffuseView = nullptr;
    ID3D11ShaderResourceView* m_specularView = nullptr;
    ID3D11ShaderResourceView* m_reflectanceView = nullptr;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/opaque.hlsl";
    inline static const wchar_t* kDepth2DShaderPath = L"Assets/Shaders/depth.hlsl";
    inline static const wchar_t* kFlashlightTexturePath = L"Assets/Textures/flashlight_1.dds";
  };
}
