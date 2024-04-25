#include "MainScene.h"

#include "Flame/math/Cube.h"
#include "Flame/math/Plane.h"
#include "Flame/math/Triangle.h"
#include "Flame/render/AlbedoMaterial.h"
#include "Flame/render/LambertianMaterial.h"
#include "Flame/render/MetalMaterial.h"

MainScene::MainScene(Flame::Window& window)
: m_window(window)
, m_input(window.GetInputSystem()) {
}

void MainScene::Initialize() {
  // TODO Move fillers to Scene. Move filling to Application
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
  m_materials.emplace_back(
   std::make_unique<Flame::MetalMaterial>(glm::vec3(0.99f, 0.99f, 0.99f))
  );

  // Create spheres
  //m_hitables.emplace_back(
  // std::make_unique<Flame::Sphere>(
  //   glm::vec3(0, 0, -2),
  //   0.5f,
  //   *m_materials[0]
  // )
  //);
  //m_hitables.emplace_back(
  // std::make_unique<Flame::Sphere>(
  //   glm::vec3(0, 1, -1),
  //   0.3f,
  //   *m_materials[1]
  // )
  //);
  m_hitables.emplace_back(
   std::make_unique<Flame::Sphere>(
     glm::vec3(0, -50.5, -1),
     50.0f,
     *m_materials[2]
   )
  );
  //m_hitables.emplace_back(
  // std::make_unique<Flame::Plane>(
  //   glm::vec3(0, 0, -5),
  //   glm::normalize(glm::vec3(1, 0.2, 1)),
  //   *m_materials[3]
  // )
  //);
  //m_hitables.emplace_back(
  // std::make_unique<Flame::Triangle>(
  //   glm::vec3(0, 2, -4),
  //   glm::vec3(2, 2, -4),
  //   glm::vec3(1, 0, -4),
  //   glm::vec3(0, 0, 1),
  //   *m_materials[4]
  // )
  //);
  m_hitables.emplace_back(
   std::make_unique<Flame::Cube>(
     *m_materials[5]
   )
  );
}

void MainScene::Update(float deltaTime) {
}

void MainScene::Cleanup() {
}

void MainScene::HandleEvent(const Flame::WindowEvent& e) {
}
