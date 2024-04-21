#pragma once

#include <memory>

#include "Flame/layers/Scene3D.h"
#include "Flame/math/HitableList.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/Sphere.h"
#include "Flame/window/InputSystem.h"

struct Window;

struct MainScene : Flame::Scene3D {
  explicit MainScene(Window& window);
  ~MainScene() override = default;

  void Initialize() override;
  void Update(float deltaTime) override;
  void Cleanup() override;
  void HandleEvent(const WindowEvent& e) override;

private:
  Window& m_window;
  InputSystem& m_input;

  // TODO Not here
  std::vector<std::unique_ptr<IHitable>> m_hitables;
  std::vector<std::unique_ptr<IMaterial>> m_materials;

  Sphere* m_sphere2 = nullptr;
  Sphere* m_grabbed = nullptr;
  float m_grabbedTime = 0.0f;
  glm::vec3 m_grabbedOffset = glm::vec3();
};
