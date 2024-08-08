#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"

namespace Flame {
  struct ModelManager final {
    std::shared_ptr<Model> GetModel(const std::string& path);
    bool LoadModel(const std::string& path);
    void Cleanup();

    static ModelManager* Get();

  private:
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
    Assimp::Importer m_importer;

    inline static uint32_t kLoadFlags = aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_RemoveRedundantMaterials;
  };
}
