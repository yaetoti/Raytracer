#pragma once

#include "Flame/engine/Model.h"
#include "Flame/engine/TransformSystem.h"
#include "Flame/utils/SolidVector.h"

#include <memory>
#include <span>

namespace Flame {
  template <typename InstanceDataType, typename MaterialDataType>
  struct ShaderGroup {

    // PerInstance

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

    // PerMaterial

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

      SolidVector<std::shared_ptr<PerInstance>>& GetInstances() {
        return m_instances;
      }

      const SolidVector<std::shared_ptr<PerInstance>>& GetInstances() const {
        return m_instances;
      }

      uint32_t AddInstance(InstanceDataType data) {
        return m_instances.emplace(std::make_shared<PerInstance>(std::move(data)));
      }

    private:
      MaterialDataType m_data;
      SolidVector<std::shared_ptr<PerInstance>> m_instances;
    };

    // PerMesh

    struct PerMesh final {
      PerMesh() = default;

      SolidVector<std::shared_ptr<PerMaterial>>& GetMaterials() {
        return m_materials;
      }

      const SolidVector<std::shared_ptr<PerMaterial>>& GetMaterials() const {
        return m_materials;
      }

      uint32_t AddMaterial(MaterialDataType data) {
        return m_materials.emplace(std::make_shared<PerMaterial>(std::move(data)));
      }

    private:
      SolidVector<std::shared_ptr<PerMaterial>> m_materials;
    };

    // PerModel

    struct PerModel final {
      PerModel(std::shared_ptr<Model> model)
      : m_model(std::move(model)) {
        for (uint32_t i = 0; i < m_model->m_meshes.size(); ++i) {
          m_meshes.emplace(std::make_shared<PerMesh>());
        }
      }

      std::shared_ptr<Model>& GetModel() {
        return m_model;
      }
      
      const std::shared_ptr<Model>& GetModel() const {
        return m_model;
      }

      SolidVector<std::shared_ptr<PerMesh>>& GetMeshes() {
        return m_meshes;
      }

      const SolidVector<std::shared_ptr<PerMesh>>& GetMeshes() const {
        return m_meshes;
      }

      void AddMaterialToAllMeshes(MaterialDataType data) {
        for (uint32_t i = 0; i < m_meshes.size(); ++i) {
          m_meshes[i]->AddMaterial(data);
        }
      }

    private:
      std::shared_ptr<Model> m_model;
      SolidVector<std::shared_ptr<PerMesh>> m_meshes;
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
        for (const auto& perMesh : perModel->GetMeshes()) {
          for (const auto& perMaterial : perMesh->GetMaterials()) {
            numInstances += perMaterial->GetInstances().size();
          }
        }
      }

      return numInstances;
    }

    uint32_t AddModel(std::shared_ptr<Model> model) {
      return m_models.emplace(std::make_shared<PerModel>(std::move(model)));
    }

    void AddInstance(std::shared_ptr<Model> model, MaterialDataType mData, InstanceDataType iData) {
      uint32_t modelId = AddModel(model);
      auto& perModel = GetModels()[modelId];

      for (auto& perMesh : perModel->GetMeshes()) {
        uint32_t materialId = perMesh->AddMaterial(mData);
        auto& perMaterial = perMesh->GetMaterials()[materialId];
        perMaterial->AddInstance(iData);
      }
    }

    void AddInstances(std::shared_ptr<Model> model, MaterialDataType mData, std::span<InstanceDataType> iDataSpan) {
      uint32_t modelId = AddModel(model);
      auto& perModel = GetModels()[modelId];

      for (auto& perMesh : perModel->GetMeshes()) {
        uint32_t materialId = perMesh->AddMaterial(mData);
        auto& perMaterial = perMesh->GetMaterials()[materialId];
        for (auto& iData : iDataSpan) {
          perMaterial->AddInstance(iData);
        }
      }
    }

    std::shared_ptr<PerModel> GetModel(uint32_t id) const {
      return m_models[id];
    }

    bool HitInstance(const Ray& ray, HitRecord<PerInstance*>& record, float tMin, float tMax) const {
      HitRecord<const Mesh*> record0;

      // Go through all instances and find the closest one
      for (const auto& perModel : GetModels()) {
        const auto& model = perModel->GetModel();

        for (uint32_t meshId = 0; meshId < perModel->GetMeshes().size(); ++meshId) {
          const auto& perMesh = perModel->GetMeshes()[meshId];

          for (const auto& perMaterial : perMesh->GetMaterials()) {
            for (const auto& perInstance : perMaterial->GetInstances()) {
              // Transform ray
              glm::mat4 modelMat = TransformSystem::Get()->At(perInstance->GetData().transformId)->transform.GetMat();
              glm::mat4 modelMatInv = glm::inverse(modelMat);

              // Transform meshToModel
              // TODO: Ok, there may be zero. If we can have more than 1 then we should multiply them all
              // TODO: Another fix: add identity if zero after parsing.
              assert(model->m_meshes[meshId].transforms.size() <= 1);
              assert(model->m_meshes[meshId].transformsInv.size() <= 1);
              if (model->m_meshes[meshId].transforms.size() != 0) {
                modelMat *= model->m_meshes[meshId].transforms[0];
                modelMatInv = model->m_meshes[meshId].transformsInv[0] * modelMatInv;
              }


              glm::vec4 position = modelMatInv * glm::vec4(ray.origin, 1.0f);
              glm::vec3 direction = modelMatInv * glm::vec4(ray.direction, 0.0f);
              Ray rayModel { position / position.w, direction };

              // Hit model in model space
              if (model->m_meshes[meshId].Hit(rayModel, record0, tMin, tMax)) {
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
      }

      return record.data != nullptr;
    }

  private:
    SolidVector<std::shared_ptr<PerModel>> m_models;
  };
}
