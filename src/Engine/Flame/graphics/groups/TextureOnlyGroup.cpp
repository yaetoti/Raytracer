#include "TextureOnlyGroup.h"

namespace Flame {
  void TextureOnlyGroup::Init() {
    // Load vertex shader
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

      m_vertexShader.Init(kShaderPath, desc, ARRAYSIZE(desc));
    }

    // Load pixel shader
    m_pixelShader.Init(kShaderPath);
  }

  void TextureOnlyGroup::Cleanup() {
    m_instanceBufferDirty = true;
    m_pixelShader.Reset();
    m_vertexShader.Reset();
    m_instanceBuffer.Reset();
    GetModels().clear();
  }

  void TextureOnlyGroup::InitInstanceBuffer() {
    m_instanceBuffer.Reset();
    HRESULT result = m_instanceBuffer.Init(GetInstanceCount(), D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    assert(SUCCEEDED(result));
  }

  void TextureOnlyGroup::UpdateInstanceBuffer() {
    // Fill buffer
    auto mapping = m_instanceBuffer.Map(D3D11_MAP_WRITE_DISCARD);
    {
      auto destPtr = static_cast<TextureOnlyInstanceData::ShaderData*>(mapping.pData);
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
    }

    m_instanceBuffer.Unmap();
  }

  void TextureOnlyGroup::Render() {
    UpdateInstanceBuffer();

    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    // Set shaders and assembly
    dc->VSSetShader(m_vertexShader.GetShader(), nullptr, 0);
    dc->PSSetShader(m_pixelShader.GetShader(), nullptr, 0);
    dc->IASetInputLayout(m_vertexShader.GetInputLayout());
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    uint32_t numRenderedInstances = 0;
    for (const auto & perModel : GetModels()) {
      const auto& model = perModel->GetModel();

      // Set buffers
      ID3D11Buffer* buffers[] = {
        model->m_vertices.Get(),
        m_instanceBuffer.Get()
      };

      UINT strides[] = {
        model->m_vertices.GetStride(),
        m_instanceBuffer.GetStride(),
      };

      UINT offsets[] = {
        0,
        0,
      };

      dc->IASetVertexBuffers(0, 2, buffers, strides, offsets);
      dc->IASetIndexBuffer(model->m_indices.Get(), DXGI_FORMAT_R32_UINT, 0);

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

          // Set texture
          dc->PSSetShaderResources(0, 1, &perMaterial->GetData().textureView);

          dc->DrawIndexedInstanced(range.indexNum, numInstances, range.indexOffset, range.vertexOffset, numRenderedInstances);
          numRenderedInstances += numInstances;
        }
      }
    }
  }
}
