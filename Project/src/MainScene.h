#pragma once

#include <memory>

#include "Flame/layers/Scene.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/Sphere.h"
#include "Flame/window/InputSystem.h"
#include "Flame/window/Window.h"

struct MainScene : Flame::Scene {
  explicit MainScene(Flame::Window& window);
  ~MainScene() override = default;

  void Initialize() override;
  void Update(float deltaTime) override;
  void Cleanup() override;
  void HandleEvent(const Flame::WindowEvent& e) override;

private:
  Flame::Window& m_window;
  Flame::InputSystem& m_input;

  // TODO Not here. ResourceManager
  std::vector<std::unique_ptr<Flame::IMaterial>> m_materials;
};
