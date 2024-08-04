#pragma once

#include "Flame.h"

#include <memory>

struct MainScene : Flame::SceneOld {
  explicit MainScene(Flame::Window& window);
  ~MainScene() override = default;

  void Initialize() override;
  void Update(float deltaTime) override;
  void Cleanup() override;

private:
  void InitializeMaterials();
  void InitializeMeshes();

private:
  Flame::Window& m_window;
  Flame::InputSystem& m_input;

  // TODO move to scene?
  std::vector<std::unique_ptr<Flame::MeshOld>> m_meshes; 
};
