#include "MainScene.h"

#include "Flame/math/Plane.h"
#include "Flame/math/Triangle.h"
#include "Flame/render/AlbedoMaterial.h"
#include "Flame/render/LambertianMaterial.h"
#include "Flame/render/MetalMaterial.h"
#include "Flame/window/events/MouseButtonWindowEvent.h"

MainScene::MainScene(Flame::Window& window)
: m_window(window)
, m_input(window.GetInput()) {
}

void MainScene::Initialize() {
  // TODO Move fill to scene
  // Create materials
  m_materials.emplace_back(
   std::make_unique<Flame::MetalMaterial>(glm::vec3(0.90f, 0.90f, 0.90f))
  );
  m_materials.emplace_back(
   std::make_unique<Flame::AlbedoMaterial>(glm::vec3(0.28f, 1.0f, 0.5f))
  );
  m_materials.emplace_back(
   std::make_unique<Flame::LambertianMaterial>(glm::vec3(1.0f, 0.28f, 0.5f))
  );
  m_materials.emplace_back(
   std::make_unique<Flame::LambertianMaterial>(glm::vec3(0.26f, 0.30f, 0.32f))
  );
  m_materials.emplace_back(
   std::make_unique<Flame::MetalMaterial>(glm::vec3(0.99f, 0.99f, 0.99f))
  );

  // Create spheres
  m_hitables.emplace_back(
   std::make_unique<Flame::Sphere>(
     glm::vec3(0, 0, -2),
     0.5f,
     *m_materials[0]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Flame::Sphere>(
     glm::vec3(0, 1, -1),
     0.3f,
     *m_materials[1]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Flame::Sphere>(
     glm::vec3(0, -50.5, -1),
     50.0f,
     *m_materials[2]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Flame::Plane>(
     glm::vec3(0, 0, -5),
     glm::normalize(glm::vec3(1, 0.2, 1)),
     *m_materials[3]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Flame::Triangle>(
     glm::vec3(1, 0, -4),
     glm::vec3(0, 2, -4),
     glm::vec3(2, 2, -4),
     glm::normalize(glm::vec3(1, 0.2, 1)),
     *m_materials[4]
   )
  );

  // TODO remove
  m_sphere2 = dynamic_cast<Flame::Sphere*>(m_hitables[1].get());
}

void MainScene::Update(float deltaTime) {
  //if (m_input.IsKeyPressed('A')) {
  //  m_sphere2->center -= glm::vec3(1.0f * deltaTime, 0.0f, 0.0f);
  //}
  //if (m_input.IsKeyPressed('D')) {
  //  m_sphere2->center -= glm::vec3(-1.0f * deltaTime, 0.0f, 0.0f);
  //}
  //if (m_input.IsKeyPressed('W')) {
  //  m_sphere2->center -= glm::vec3(0, -1.0f * deltaTime, 0.0f);
  //}
  //if (m_input.IsKeyPressed('S')) {
  //  m_sphere2->center -= glm::vec3(0, 1.0f * deltaTime, 0.0f);
  //}

  //if (m_input.IsMouseButtonPressed(Flame::MouseButton::RIGHT)) {
  //  auto[x, y] = m_input.GetCursorPos();
  //  auto[lastX, lastY] = m_input.GetLastCursorPos();
  //  m_sphere2->center -= glm::vec3((lastX - x) * deltaTime, -(lastY - y) * deltaTime, 0.0f);
  //}
}

void MainScene::Cleanup() {
}

void MainScene::HandleEvent(const Flame::WindowEvent& e) {
}
