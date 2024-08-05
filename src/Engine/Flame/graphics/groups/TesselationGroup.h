#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Flame/engine/Transform.h"
#include "Flame/graphics/buffers/VertexBuffer.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "Flame/math/HitRecord.h"

namespace Flame {
  struct TesselationGroup final {
    struct InstanceData final {
      struct ShaderData final {
        glm::mat4 modelMatrix;
      };

      ShaderData GetShaderData() const {
        
        ShaderData data;
        data.modelMatrix = transform.GetMat();
        return data;
      }

    public:
      Transform transform;
    };

    struct MaterialData final {
      // Empty for now
    };

    struct PerInstance final {
      std::string name;
      InstanceData data;
    };

    struct PerMaterial final {
      PerMaterial() = default;
      PerMaterial(std::string name, const MaterialData& data);

      const std::vector<std::shared_ptr<PerInstance>>& GetInstances() const;

      std::shared_ptr<PerInstance> GetInstance(const std::string& name);
      std::shared_ptr<PerInstance>& AddInstance(const std::string& name, const InstanceData& data);

    public:
      std::string name;
      MaterialData data;

    private:
      std::vector<std::shared_ptr<PerInstance>> m_perInstance;
    };

    struct PerModel final {
      PerModel() = default;
      PerModel(std::string name, std::shared_ptr<Model> model);

      const std::vector<std::shared_ptr<PerMaterial>>& GetMaterials() const;

      std::shared_ptr<PerMaterial> GetMaterial(const std::string& name) const;
      std::shared_ptr<PerMaterial>& AddMaterial(const std::string& name, const MaterialData& data);

    public:
      std::string name;
      std::shared_ptr<Model> model;

    private:
      std::vector<std::shared_ptr<PerMaterial>> m_perMaterial;
    };

    std::shared_ptr<PerModel> GetModel(const std::string& name);
    std::shared_ptr<PerModel>& AddModel(const std::string& name, std::shared_ptr<Model> model);

    void Init();
    void Cleanup();

    /// HitRecord.hitable = PerInstance
    bool HitInstance(const Ray& ray, HitRecord<PerInstance*>& record, float tMin, float tMax) const;
    uint32_t GetInstanceCount() const;
    void UpdateInstanceBuffer();
    void Render();

  private:
    std::vector<std::shared_ptr<PerModel>> m_perModel;
    VertexBuffer<InstanceData::ShaderData> m_instanceBuffer;
    bool m_instanceBufferDirty = true;

    VertexShader m_vertexShader;
    PixelShader m_pixelShader;

    inline static const wchar_t* kShaderPath = L"Assets/Shaders/hologram.hlsl";
  };
}
