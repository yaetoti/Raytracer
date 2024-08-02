#include "ModelManager.h"

namespace Flame {
  std::shared_ptr<Model> ModelManager::GetModel(const std::string& path) {
    if (!m_models.contains(path)) {
      if (!LoadModel(path)) {
        return nullptr;
      }
    }

    return m_models.at(path);
  }

  bool ModelManager::LoadModel(const std::string& path) {
    if (m_models.contains(path)) {
      return true;
    }

    auto scene = m_importer.ReadFile(path.c_str(), kLoadFlags);
    assert(scene && (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 1);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
      return false;
    }

    auto model = std::make_shared<Model>();
    model->Parse(*scene);
    m_models.emplace(path, std::move(model));
    m_importer.FreeScene();

    return true;
  }

  void ModelManager::Cleanup() {
    m_models.clear();
  }

  ModelManager* ModelManager::Get() {
    static ModelManager instance;
    return &instance;
  }
}
