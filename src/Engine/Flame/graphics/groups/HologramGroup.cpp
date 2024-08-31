#include "HologramGroup.h"

namespace Flame {
  void HologramGroup::Init() {
    // Load vertex shader
    {
      D3D11_INPUT_ELEMENT_DESC desc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "MODEL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MODEL", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "MAIN_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "SECONDARY_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
      };

      m_vertexShader.Init(kShaderPath, desc, ARRAYSIZE(desc));
    }

    // Load other shaders
    m_pixelShader.Init(kShaderPath);
    m_geometryShader.Init(kShaderPath);
    m_hullShader.Init(kShaderPath);
    m_domainShader.Init(kShaderPath);
  }

  void HologramGroup::Cleanup() {
    m_instanceBufferDirty = true;
    m_domainShader.Reset();
    m_hullShader.Reset();
    m_geometryShader.Reset();
    m_pixelShader.Reset();
    m_vertexShader.Reset();
    m_instanceBuffer.Reset();
    GetModels().clear();
  }

  void HologramGroup::InitInstanceBuffer() {
    m_instanceBuffer.Reset();
    HRESULT result = m_instanceBuffer.Init(GetInstanceCount(), D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    assert(SUCCEEDED(result));
  }

  void HologramGroup::UpdateInstanceBuffer() {
    // Fill buffer
    auto mapping = m_instanceBuffer.Map(D3D11_MAP_WRITE_DISCARD);
    {
      auto destPtr = static_cast<HologramInstanceData::ShaderData*>(mapping.pData);
      uint32_t numCopied = 0;

      for (const auto & perModel : GetModels()) {
        for (const auto & perMaterial : perModel->GetMaterials()) {
          for (const auto & perInstance : perMaterial->GetInstances()) {
            destPtr[numCopied++] = perInstance->GetData().GetShaderData();
          }
        }
      }
    }

    m_instanceBuffer.Unmap();
  }

  void HologramGroup::Render() {
    UpdateInstanceBuffer();

    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    // Set shaders and assembly
    dc->VSSetShader(m_vertexShader.GetShader(), nullptr, 0);
    dc->PSSetShader(m_pixelShader.GetShader(), nullptr, 0);
    dc->GSSetShader(m_geometryShader.GetShader(), nullptr, 0);
    dc->HSSetShader(m_hullShader.GetShader(), nullptr, 0);
    dc->DSSetShader(m_domainShader.GetShader(), nullptr, 0);
    dc->IASetInputLayout(m_vertexShader.GetInputLayout());
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

    uint32_t numRenderedInstances = 0;
    for (const auto & perModel : GetModels()) {
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

      for (const auto & perMaterial : perModel->GetMaterials()) {
        const auto& instances = perMaterial->GetInstances();
        uint32_t numInstances = instances.size();
        if (numInstances == 0) {
          continue;
        }

        // Aaah, so that's why we have MeshRange... Finally
        // TODO replace Model index offsetting with correct draw call parameters
        dc->DrawIndexedInstanced(perModel->GetModel()->m_indexNum, numInstances, 0, 0, numRenderedInstances);
        numRenderedInstances += numInstances;
      }
    }

    dc->VSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
  }
}
