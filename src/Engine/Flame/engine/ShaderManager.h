#pragma once
#include <memory>
#include <unordered_map>

#include "ShaderPipeline.h"

namespace Flame {
  struct ShaderManager final {
    void Init();
    void Cleanup();

    void AddPipeline(const std::wstring& name, std::shared_ptr<ShaderPipeline> pipeline);
    std::shared_ptr<ShaderPipeline> GetPipeline(const std::wstring& name);

    static ShaderManager* Get();

  private:
    std::unordered_map<std::wstring, std::shared_ptr<ShaderPipeline>> m_shaders;
  };
}
