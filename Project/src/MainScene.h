#pragma once

#include <memory>

#include "Flame/layers/Scene3D.h"
#include "Flame/math/HitableList.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/Sphere.h"
#include "Flame/window/InputSystem.h"
#include "Flame/window/Window.h"

struct MainScene : Flame::Scene3D {
  explicit MainScene(Flame::Window& window);
  ~MainScene() override = default;

  void Initialize() override;
  void Update(float deltaTime) override;
  void Cleanup() override;
  void HandleEvent(const Flame::WindowEvent& e) override;

private:
  Flame::Window& m_window;
  Flame::InputSystem& m_input;

  // TODO Not here
  std::vector<std::unique_ptr<Flame::IHitable>> m_hitables;
  std::vector<std::unique_ptr<Flame::IMaterial>> m_materials;

  Flame::Sphere* m_sphere2 = nullptr;
  Flame::Sphere* m_grabbed = nullptr;
  float m_grabbedTime = 0.0f;
  glm::vec3 m_grabbedOffset = glm::vec3();
};
