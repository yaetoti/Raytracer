#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>
#include <Flame/engine/IShadowMapProvider.h>
#include <Flame/engine/ShaderPipeline.h>
#include <Flame/graphics/buffers/data/DepthCubemapData.h>
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

    struct DepthShaderData final {
      glm::mat4 modelMatrix;
    };

    ShaderData GetShaderData() const {
      return ShaderData {
        TransformSystem::Get()->At(transformId)->transform.GetMat()
      };
    }

    DepthShaderData GetDepthShaderData() const {
      return DepthShaderData {
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

    void SetShadowMapProvider(const std::shared_ptr<IShadowMapProvider>& provider);

  private:
    void UpdateInstanceBufferData();
    void UpdateInstanceBuffer();
    void UpdateInstanceBufferDataDepth();
    void UpdateInstanceBufferDepth();

  private:
    ShaderPipeline m_pipeline;
    ShaderPipeline m_pipelineDepth2D;
    ShaderPipeline m_pipelineDepthCubemap;
    VertexBuffer<OpaqueInstanceData::ShaderData> m_instanceBuffer;
    VertexBuffer<OpaqueInstanceData::DepthShaderData> m_instanceBufferDepth;
    uint32_t m_instanceCount = 0;
    uint32_t m_instanceCountDepth = 0;
    ConstantBuffer<OpaqueMeshData> m_meshBuffer;
    // TODO make some global structure to use in different groups
    ConstantBuffer<DepthCubemapData> m_cubemapDepthBuffer;

    // IBL
    ID3D11ShaderResourceView* m_diffuseView = nullptr;
    ID3D11ShaderResourceView* m_specularView = nullptr;
    ID3D11ShaderResourceView* m_reflectanceView = nullptr;

    // DI
    std::shared_ptr<IShadowMapProvider> m_shadowMapProvider;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/Opaque.hlsl";
    inline static const wchar_t* kDepth2DShaderPath = L"Assets/Shaders/Depth2D.hlsl";
    inline static const wchar_t* kDepthCubemapShaderPath = L"Assets/Shaders/DepthCubemap.hlsl";
    inline static const wchar_t* kFlashlightTexturePath = L"Assets/Textures/flashlight_1.dds";
  };
}
