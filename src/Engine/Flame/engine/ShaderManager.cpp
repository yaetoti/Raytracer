#include "ShaderManager.h"

namespace Flame {
  void ShaderManager::Init() {
  }

  void ShaderManager::Cleanup() {
  }

  void ShaderManager::AddPipeline(const std::wstring& name, std::shared_ptr<ShaderPipeline> pipeline) {
    m_shaders.at(name) = std::move(pipeline);
  }

  std::shared_ptr<ShaderPipeline> ShaderManager::GetPipeline(const std::wstring& name) {
    return m_shaders.at(name);
  }

  ShaderManager* ShaderManager::Get() {
    static ShaderManager instance;
    return &instance;
  }
}
