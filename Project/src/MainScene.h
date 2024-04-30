#pragma once

#include <memory>

#include "Flame/layers/Scene.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/Mesh.h"
#include "Flame/math/Sphere.h"
#include "Flame/window/InputSystem.h"
#include "Flame/window/Window.h"

struct MainScene : Flame::Scene {
  explicit MainScene(Flame::Window& window);
  void InitializeMeshes();
  ~MainScene() override = default;

  void Initialize() override;
  void Update(float deltaTime) override;
  void Cleanup() override;

private:
  void InitializeMaterials();

private:
  Flame::Window& m_window;
  Flame::InputSystem& m_input;

  // TODO move to scene?
  std::vector<std::unique_ptr<Flame::Mesh>> m_meshes; 
};
