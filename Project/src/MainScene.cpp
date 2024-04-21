#include "MainScene.h"

#include "Flame/math/Plane.h"
#include "Flame/render/AlbedoMaterial.h"
#include "Flame/render/LambertianMaterial.h"
#include "Flame/render/MetalMaterial.h"
#include "Flame/window/events/MouseButtonWindowEvent.h"

MainScene::MainScene(Flame::Window& window)
: m_window(window)
, m_input(window.GetInput()) {
}

void MainScene::Initialize() {
  Layer::Initialize();

    // Create materials
  m_materials.emplace_back(
   std::make_unique<Flame::MetalMaterial>(glm::vec3(0.98f, 0.98f, 0.98f))
  );
  m_materials.emplace_back(
   std::make_unique<Flame::AlbedoMaterial>(glm::vec3(0.28f, 1.0f, 0.5f))
  );
  m_materials.emplace_back(
   std::make_unique<Flame::LambertianMaterial>(glm::vec3(1.0f, 0.28f, 0.5f))
  );
  m_materials.emplace_back(
   std::make_unique<Flame::AlbedoMaterial>(glm::vec3(0.16f, 0.10f, 0.22f))
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
     glm::normalize(glm::vec3(1, 0, 1)),
     *m_materials[3]
   )
  );

  m_sphere2 = dynamic_cast<Flame::Sphere*>(m_hitables[1].get());
}

void MainScene::Update(float deltaTime) {
  Layer::Update(deltaTime);

  if (m_input.IsKeyPressed('A')) {
    m_sphere2->center -= glm::vec3(1.0f * deltaTime, 0.0f, 0.0f);
  }
  if (m_input.IsKeyPressed('D')) {
    m_sphere2->center -= glm::vec3(-1.0f * deltaTime, 0.0f, 0.0f);
  }
  if (m_input.IsKeyPressed('W')) {
    m_sphere2->center -= glm::vec3(0, -1.0f * deltaTime, 0.0f);
  }
  if (m_input.IsKeyPressed('S')) {
    m_sphere2->center -= glm::vec3(0, 1.0f * deltaTime, 0.0f);
  }

  if (m_input.IsMouseButtonPressed(Flame::MouseButton::RIGHT)) {
    auto[x, y] = m_input.GetCursorPos();
    auto[lastX, lastY] = m_input.GetLastCursorPos();
    m_sphere2->center -= glm::vec3((lastX - x) * deltaTime, -(lastY - y) * deltaTime, 0.0f);
  }

  // TODO Grabbing
  
  auto[x, y] = m_input.GetCursorPos();
  glm::mat4 perspective = glm::ortho(
    0.0f,
    static_cast<float>(m_window.GetWidth()),
    static_cast<float>(m_window.GetHeight()),
    0.0f);
  glm::mat4 inversed = glm::inverse(perspective);
  glm::vec4 pos(x, y, 0.0f, 1.0f);
  // std::cout << pos << " " << perspective * pos << '\n';

  if (m_input.IsMouseButtonPressed(Flame::MouseButton::LEFT)) {
    if (m_grabbed == nullptr) {
      Flame::Ray ray(glm::vec3(perspective * pos), glm::vec3(0.0f, 0.0f, -1.0f));
      Flame::HitRecord record;
      if (m_sphere2->Hit(ray, 0.0f, 1000.0f, record)) {
        m_grabbed = m_sphere2;
        m_grabbedTime = record.time;
        m_grabbedOffset = m_sphere2->center - record.point;
      }
    }
  } else {
    m_grabbed = nullptr;
  }

  if (m_grabbed != nullptr) {
    m_grabbed->center = glm::vec3(perspective * pos) + glm::vec3(0.0f, 0.0f, -1.0f) * m_grabbedTime + m_grabbedOffset;
  }
}

void MainScene::Cleanup() {
  Layer::Cleanup();
}

void MainScene::HandleEvent(const Flame::WindowEvent& e) {
  Layer::HandleEvent(e);
}
