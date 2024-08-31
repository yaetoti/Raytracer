#include "OpaqueGroup.h"

namespace Flame {
  void OpaqueGroup::Init() {
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
      };

      m_vertexShader.Init(kShaderPath, desc, ARRAYSIZE(desc));
    }

    // Load pixel shader
    m_pixelShader.Init(kShaderPath);

    // TODO FIX. This creates the need for initialization only after parsing the scene graph
    // Create instance buffer
    HRESULT result;
    result = m_instanceBuffer.Init(GetInstanceCount(), D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    assert(SUCCEEDED(result));
  }

  void OpaqueGroup::Cleanup() {
    m_instanceBufferDirty = true;
    m_pixelShader.Reset();
    m_vertexShader.Reset();
    m_instanceBuffer.Reset();
    GetModels().clear();
  }

  bool OpaqueGroup::HitInstance(const Ray& ray, HitRecord<PerInstance*>& record, float tMin, float tMax) const {
    HitRecord<const Model*> record0;

    // Go through all instances and find the closest one
    for (const auto & perModel : GetModels()) {
      const auto& model = *perModel->GetModel();

      for (const auto & perMaterial : perModel->GetMaterials()) {
        for (const auto & perInstance : perMaterial->GetInstances()) {
          // TODO we could store these as we use them often, but definitely not in PerInstance as we copy it entirely into the buffer
          // Transform ray
          const glm::mat4& modelMat = perInstance->GetData().transform.GetMat();
          glm::mat4 modelMatInv = glm::inverse(modelMat);
          glm::vec4 position = modelMatInv * glm::vec4(ray.origin, 1.0f);
          glm::vec3 direction = modelMatInv * glm::vec4(ray.direction, 0.0f);
          Ray rayModel { position / position.w, direction };

          // Hit model in model space
          if (model.Hit(rayModel, record0, tMin, tMax)) {
            // If hit - update tMax and InvTransform the results
            tMax = record0.time;
            position = modelMat * glm::vec4(record0.point, 1.0f);
            record.time = tMax;
            record.point = position / position.w;
            record.normal = glm::normalize(modelMat * glm::vec4(record0.normal, 0.0f));
            record.data = perInstance.get();
          }
        }
      }
    }

    return record.data != nullptr;
  }

  void OpaqueGroup::UpdateInstanceBuffer() {
    // Fill buffer
    auto mapping = m_instanceBuffer.Map(D3D11_MAP_WRITE_DISCARD);
    {
      auto destPtr = static_cast<OpaqueInstanceData::ShaderData*>(mapping.pData);
      uint32_t numCopied = 0;

      for (const auto& perModel : GetModels()) {
        for (const auto& perMaterial : perModel->GetMaterials()) {
          for (const auto& perInstance : perMaterial->GetInstances()) {
            destPtr[numCopied++] = perInstance->GetData().GetShaderData();
          }
        }
      }
    }

    m_instanceBuffer.Unmap();
  }

  void OpaqueGroup::Render() {
    UpdateInstanceBuffer();

    ID3D11DeviceContext* dc = DxContext::Get()->d3d11DeviceContext.Get();
    // Set shaders and assembly
    dc->VSSetShader(m_vertexShader.GetShader(), nullptr, 0);
    dc->PSSetShader(m_pixelShader.GetShader(), nullptr, 0);
    dc->IASetInputLayout(m_vertexShader.GetInputLayout());
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
  }
}
