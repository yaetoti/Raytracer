#include "OpaqueGroup.h"
#include "Flame/engine/TextureManager.h"
#include <d3d11.h>
#include <Flame/engine/Engine.h>
#include <Flame/graphics/buffers/CBufferIndices.h>

namespace Flame {
  void OpaqueGroup::Init() {
    {
      D3D11_INPUT_ELEMENT_DESC desc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "MODEL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
      };

      m_pipeline.Init(kShaderPath, ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
      m_pipeline.CreateInputLayout(desc);
    }

    {
      D3D11_INPUT_ELEMENT_DESC desc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "MODEL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
      };

      m_pipelineDepth2D.Init(kDepth2DShaderPath, ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
      m_pipelineDepth2D.CreateInputLayout(desc);
    }

    m_meshBuffer.Init();

    m_diffuseView = TextureManager::Get()->GetTexture(Engine::GetDirectory(L"Generated\\Textures\\IBL\\diffuse.dds"))->GetResourceView();
    m_specularView = TextureManager::Get()->GetTexture(Engine::GetDirectory(L"Generated\\Textures\\IBL\\specular.dds"))->GetResourceView();
    m_reflectanceView = TextureManager::Get()->GetTexture(Engine::GetDirectory(L"Generated\\Textures\\IBL\\reflectance.dds"))->GetResourceView();
  }

  void OpaqueGroup::Cleanup() {
    m_shadowMapProvider.reset();

    // Shaders
    m_pipeline.Reset();
    m_pipelineDepth2D.Reset();
    // Instance (vertex) buffers
    m_instanceBuffer.Reset();
    m_instanceBufferDepth2D.Reset();
    m_instanceCount = 0;

    m_meshBuffer.Reset();
    GetModels().clear();
  }

  void OpaqueGroup::SetShadowMapProvider(const std::shared_ptr<IShadowMapProvider>& provider) {
    m_shadowMapProvider = provider;
  }

  void OpaqueGroup::UpdateInstanceBufferData() {
    auto mapping = m_instanceBuffer.Map(D3D11_MAP_WRITE_DISCARD);
    auto destPtr = static_cast<OpaqueInstanceData::ShaderData*>(mapping.pData);
    uint32_t numCopied = 0;

    for (const auto& perModel : GetModels()) {
      for (const auto & perMesh : perModel->GetMeshes()) {
        for (const auto & perMaterial : perMesh->GetMaterials()) {
          for (const auto & perInstance : perMaterial->GetInstances()) {
            destPtr[numCopied++] = perInstance->GetData().GetShaderData();
          }
        }
      }
    }

    m_instanceBuffer.Unmap();
  }

  void OpaqueGroup::UpdateInstanceBuffer() {
    uint32_t instanceCount = GetInstanceCount();
    if (m_instanceCount != instanceCount) {
      m_instanceCount = instanceCount;
      HRESULT result = m_instanceBuffer.Init(m_instanceCount, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
      assert(SUCCEEDED(result));
    }

    UpdateInstanceBufferData();
  }

  void OpaqueGroup::UpdateInstanceBufferDataDepth2D() {
    auto mapping = m_instanceBufferDepth2D.Map(D3D11_MAP_WRITE_DISCARD);
    auto destPtr = static_cast<OpaqueInstanceData::Depth2DShaderData*>(mapping.pData);
    uint32_t numCopied = 0;

    for (const auto& perModel : GetModels()) {
      for (const auto & perMesh : perModel->GetMeshes()) {
        for (const auto & perMaterial : perMesh->GetMaterials()) {
          for (const auto & perInstance : perMaterial->GetInstances()) {
            destPtr[numCopied++] = perInstance->GetData().GetDepth2DShaderData();
          }
        }
      }
    }

    m_instanceBufferDepth2D.Unmap();
  }

  void OpaqueGroup::UpdateInstanceBufferDepth2D() {
    uint32_t instanceCount = GetInstanceCount();
    if (m_instanceCountDepth2D != instanceCount) {
      m_instanceCountDepth2D = instanceCount;
      HRESULT result = m_instanceBufferDepth2D.Init(m_instanceCountDepth2D, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
      assert(SUCCEEDED(result));
    }

    UpdateInstanceBufferDataDepth2D();
  }

  void OpaqueGroup::Render() {
    UpdateInstanceBuffer();

    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    // Set shaders and assembly
    m_pipeline.Bind();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->VSSetConstantBuffers(kMeshCBufferId, 1, m_meshBuffer.GetAddressOf());

    // Set light texture
    dc->PSSetShaderResources(0, 1, TextureManager::Get()->GetTexture(kFlashlightTexturePath)->GetResourceViewAddress());
    // Set IBL textures
    assert(m_shadowMapProvider != nullptr);
    ID3D11ShaderResourceView* iblTextures[] = {
      m_diffuseView,
      m_specularView,
      m_reflectanceView,
      m_shadowMapProvider->GetShadowMapSrvDirect()
    };
    dc->PSSetShaderResources(5, 4, iblTextures);

    uint32_t numRenderedInstances = 0;
    for (const auto & perModel : GetModels()) {
      const auto& model = perModel->GetModel();

      // Set buffers
      ID3D11Buffer* buffers[] = {
        perModel->GetModel()->m_vertices.Get(),
        m_instanceBuffer.Get()
      };

      UINT strides[] = {
        perModel->GetModel()->m_vertices.GetStride(),
        m_instanceBuffer.GetStride(),
      };

      UINT offsets[] = {
        0,
        0,
      };

      dc->IASetVertexBuffers(0, 2, buffers, strides, offsets);
      dc->IASetIndexBuffer(perModel->GetModel()->m_indices.Get(), DXGI_FORMAT_R32_UINT, 0);

      const auto& perMeshArray = perModel->GetMeshes();
      for (uint32_t meshId = 0; meshId < perMeshArray.size(); ++meshId) {
        const auto& perMesh = perMeshArray[meshId];
        // Upload mesh matrices

        // TODO: Didn't encountered such situation but looks like it may happen
        assert(model->m_meshes[meshId].transforms.size() == 1);
        assert(model->m_meshes[meshId].transformsInv.size() == 1);
        m_meshBuffer.data.meshToModel = model->m_meshes[meshId].transforms[0];
        m_meshBuffer.data.modelToMesh = model->m_meshes[meshId].transformsInv[0];
        m_meshBuffer.ApplyChanges();

        for (const auto & perMaterial : perMesh->GetMaterials()) {
          const auto& instances = perMaterial->GetInstances();
          const auto& range = model->m_ranges[meshId];
          uint32_t numInstances = instances.size();
          if (numInstances == 0) {
            continue;
          }

          // Set material
          ID3D11ShaderResourceView* srvs[] {
            perMaterial->GetData().m_albedoView,
            perMaterial->GetData().m_normalView,
            perMaterial->GetData().m_metallicView,
            perMaterial->GetData().m_roughnessView,
          };
          dc->PSSetShaderResources(1, 4, srvs);

          dc->DrawIndexedInstanced(range.indexNum, numInstances, range.indexOffset, range.vertexOffset, numRenderedInstances);
          numRenderedInstances += numInstances;
        }
      }
    }
  }

  void OpaqueGroup::RenderDepth2D() {
    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();

    UpdateInstanceBufferDepth2D();

    m_pipelineDepth2D.Bind();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->VSSetConstantBuffers(kMeshCBufferId, 1, m_meshBuffer.GetAddressOf());

    uint32_t numRenderedInstances = 0;
    for (const auto & perModel : GetModels()) {
      const auto& model = perModel->GetModel();

      // Set buffers
      ID3D11Buffer* buffers[] = {
        perModel->GetModel()->m_vertices.Get(),
        m_instanceBufferDepth2D.Get()
      };

      UINT strides[] = {
        perModel->GetModel()->m_vertices.GetStride(),
        m_instanceBufferDepth2D.GetStride(),
      };

      UINT offsets[] = {
        0,
        0,
      };

      dc->IASetVertexBuffers(0, 2, buffers, strides, offsets);
      dc->IASetIndexBuffer(perModel->GetModel()->m_indices.Get(), DXGI_FORMAT_R32_UINT, 0);

      const auto& perMeshArray = perModel->GetMeshes();
      for (uint32_t meshId = 0; meshId < perMeshArray.size(); ++meshId) {
        const auto& perMesh = perMeshArray[meshId];
        // Upload mesh matrices

        // TODO: Didn't encountered such situation but looks like it may happen
        assert(model->m_meshes[meshId].transforms.size() == 1);
        assert(model->m_meshes[meshId].transformsInv.size() == 1);
        m_meshBuffer.data.meshToModel = model->m_meshes[meshId].transforms[0];
        m_meshBuffer.data.modelToMesh = model->m_meshes[meshId].transformsInv[0];
        m_meshBuffer.ApplyChanges();

        for (const auto & perMaterial : perMesh->GetMaterials()) {
          const auto& instances = perMaterial->GetInstances();
          const auto& range = model->m_ranges[meshId];
          uint32_t numInstances = instances.size();
          if (numInstances == 0) {
            continue;
          }

          dc->DrawIndexedInstanced(range.indexNum, numInstances, range.indexOffset, range.vertexOffset, numRenderedInstances);
          numRenderedInstances += numInstances;
        }
      }
    }
  }
}
