#include "Engine.h"

#include <Flame/graphics/DxContext.h>
#include <Flame/graphics/PostProcess.h>

#include "LightSystem.h"
#include "MeshSystem.h"
#include "ModelManager.h"
#include "TextureManager.h"

namespace Flame {
  void Engine::Init() {
    DxContext::Get()->Init();
    ModelManager::Get()->Init();
    MeshSystem::Get()->Init();
    LightSystem::Get()->Init();
    PostProcess::Get()->Init();
  }

  void Engine::Cleanup() {
    PostProcess::Get()->Cleanup();
    TextureManager::Get()->Cleanup();
    LightSystem::Get()->Cleanup();
    MeshSystem::Get()->Cleanup();
    MeshSystem::Get()->Cleanup();
    ModelManager::Get()->Cleanup();
    TransformSystem::Get()->Cleanup();
    DxContext::Get()->Cleanup();
  }

  void Engine::SetWorkingDirectory(std::wstring workingDirectory) {
    m_workingDirectory = std::move(workingDirectory);
  }

  std::wstring Engine::GetWorkingDirectory() {
    return m_workingDirectory;
  }

  std::wstring Engine::GetDirectory(const std::wstring& directory) {
    return m_workingDirectory + directory;
  }
}
