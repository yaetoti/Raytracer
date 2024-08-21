#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include "Model.h"

namespace Flame {
  struct ModelManager final {
    enum class BuiltinModelType : uint32_t {
      UNIT_SPHERE,
      //UNIT_SPHERE_FLAT,
      COUNT
    };

    std::shared_ptr<Model> GetModel(const std::string& path);
    std::shared_ptr<Model> GetBuiltinModel(BuiltinModelType type);
    bool LoadModel(const std::string& path);
    void Init();
    void Cleanup();

    static ModelManager* Get();

  private:
    std::shared_ptr<Model> GenerateUnitSphereModel() const;
    std::shared_ptr<Model> GenerateFlatUnitSphereModel() const;

  private:
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    std::vector<std::shared_ptr<Model>> m_builtinModels;
    Assimp::Importer m_importer;

    inline static uint32_t kLoadFlags = aiProcess_JoinIdenticalVertices
                                        | aiProcess_Triangulate
                                        | aiProcess_ConvertToLeftHanded
                                        | aiProcess_GenBoundingBoxes
                                        | aiProcess_GenNormals
                                        | aiProcess_GenUVCoords
                                        | aiProcess_RemoveRedundantMaterials
                                        | aiProcess_CalcTangentSpace
                                        | aiProcess_ValidateDataStructure
                                        | aiProcess_FindDegenerates // All of them
                                        | aiProcess_FindInvalidData;
  };
}
