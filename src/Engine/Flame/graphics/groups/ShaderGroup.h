#pragma once

#include "Flame/engine/Model.h"
#include "Flame/utils/SolidVector.h"

#include <vector>
#include <memory>

namespace Flame {
  template <typename InstanceDataType, typename MaterialDataType>
  struct ShaderGroup {
    struct PerInstance final {
      PerInstance(InstanceDataType data)
      : m_data(std::move(data)) {
      }

      InstanceDataType& GetData() {
        return m_data;
      }

      const InstanceDataType& GetData() const {
        return m_data;
      }

    private:
      InstanceDataType m_data;
    };

    struct PerMaterial final {
      PerMaterial(MaterialDataType data)
      : m_data(std::move(data)) {
      }

      MaterialDataType& GetData() {
        return m_data;
      }
      
      const MaterialDataType& GetData() const {
        return m_data;
      }

      std::vector<std::shared_ptr<PerInstance>>& GetInstances() {
        return m_instances;
      }

      const std::vector<std::shared_ptr<PerInstance>>& GetInstances() const {
        return m_instances;
      }

      std::shared_ptr<PerInstance> AddInstance(InstanceDataType data) {
        return m_instances.emplace_back(std::make_shared<PerInstance>(std::move(data)));
      }

    private:
      MaterialDataType m_data;
      std::vector<std::shared_ptr<PerInstance>> m_instances;
    };

    struct PerModel final {
      PerModel(std::shared_ptr<Model> model)
      : m_model(std::move(model)) {
      }

      std::shared_ptr<Model>& GetModel() {
        return m_model;
      }
      
      const std::shared_ptr<Model>& GetModel() const {
        return m_model;
      }

      std::vector<std::shared_ptr<PerMaterial>>& GetMaterials() {
        return m_materials;
      }

      const std::vector<std::shared_ptr<PerMaterial>>& GetMaterials() const {
        return m_materials;
      }

      std::shared_ptr<PerMaterial> AddMaterial(MaterialDataType data) {
        return m_materials.emplace_back(std::make_shared<PerMaterial>(std::move(data)));
      }

    private:
      std::shared_ptr<Model> m_model;
      std::vector<std::shared_ptr<PerMaterial>> m_materials;
    };

    // ShaderGroup
    virtual ~ShaderGroup() = default;

    SolidVector<std::shared_ptr<PerModel>>& GetModels() {
      return m_models;
    }

    const SolidVector<std::shared_ptr<PerModel>>& GetModels() const {
      return m_models;
    }

    size_t GetInstanceCount() const {
      size_t numInstances = 0;
      for (const auto& perModel : GetModels()) {
        for (const auto& perMaterial : perModel->GetMaterials()) {
          numInstances += perMaterial->GetInstances().size();
        }
      }

      return numInstances;
    }

    uint32_t AddModel(std::shared_ptr<Model> model) {
      return m_models.emplace(std::make_shared<PerModel>(std::move(model)));
    }

    std::shared_ptr<PerModel> GetModel(uint32_t id) const {
      return m_models[id];
    }

  private:
    SolidVector<std::shared_ptr<PerModel>> m_models;
  };
}
