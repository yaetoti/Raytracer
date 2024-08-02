#pragma once
#include "MeshSystem.h"
#include "ModelManager.h"

namespace Flame {
  struct Engine final {
    static void Init() {
      DxContext::Get()->Init();
      MeshSystem::Get()->Init();
    }

    static void Cleanup() {
      MeshSystem::Get()->Cleanup();
      ModelManager::Get()->Cleanup();
      DxContext::Get()->Cleanup();
    }
  };
}
