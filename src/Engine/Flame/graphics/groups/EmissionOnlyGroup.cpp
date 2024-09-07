#include "EmissionOnlyGroup.h"

namespace Flame {
  void EmissionOnlyGroup::Init() {
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
      { "EMISSION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };

    m_pipeline.Init(kShaderPath, ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
    m_pipeline.CreateInputLayout(desc);
  }

  void EmissionOnlyGroup::Cleanup() {
    m_pipeline.Reset();
    m_instanceBuffer.Reset();
    m_instanceCount = 0;
    GetModels().clear();
  }

  void EmissionOnlyGroup::UpdateInstanceBufferData() {
    auto mapping = m_instanceBuffer.Map(D3D11_MAP_WRITE_DISCARD);
    auto destPtr = static_cast<EmissionOnlyInstanceData::ShaderData*>(mapping.pData);
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

  void EmissionOnlyGroup::UpdateInstanceBuffer() {
    uint32_t instanceCount = GetInstanceCount();
    if (m_instanceCount != instanceCount) {
      m_instanceCount = instanceCount;
      HRESULT result = m_instanceBuffer.Init(m_instanceCount, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
      assert(SUCCEEDED(result));
    }

    UpdateInstanceBufferData();
  }

  void EmissionOnlyGroup::Render() {
    UpdateInstanceBuffer();

    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    // Set shaders and assembly
    m_pipeline.Bind();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

        for (const auto & perMaterial : perMesh->GetMaterials()) {
          const auto& instances = perMaterial->GetInstances();
          const auto& range = model->m_ranges[meshId];
          uint32_t numInstances = instances.size();
          if (numInstances == 0) {
            continue;
          }

          // Aaah, so that's why we have MeshRange... Finally
          // TODO replace Model index offsetting with correct draw call parameters
          dc->DrawIndexedInstanced(range.indexNum, numInstances, range.indexOffset, range.vertexOffset, numRenderedInstances);
          numRenderedInstances += numInstances;
        }
      }
    }
  }
}
