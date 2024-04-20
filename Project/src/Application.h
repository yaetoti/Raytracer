#pragma once

#include <chrono>
#include <ConsoleLib.h>
#include <Flame.h>
#include <random>
#include <thread>
#include <Windows.h>

struct Application final : EventListener<WindowEvent> {
  Application();

  int RunMainLoop();
  void Handle(const WindowEvent* e) override;

private:
  std::shared_ptr<Window> m_window;
  Framebuffer framebuffer;

  std::shared_ptr<Sphere> sphere1 = std::make_shared<Sphere>(
    glm::vec3(0, 0, -2),
    0.5f,
    std::make_unique<MetalMaterial>(
      glm::vec3(0.98f, 0.98f, 0.98f)
    )
  );
  std::shared_ptr<Sphere> sphere2 = std::make_shared<Sphere>(
    glm::vec3(0.8f, 0.2f, -1.0f),
    0.4f,
    std::make_unique<AlbedoMaterial>(
      glm::vec3(0.28f, 1.0f, 0.5f)
    )
  );
  std::shared_ptr<Sphere> sphere3 = std::make_shared<Sphere>(
    glm::vec3(0, -10.5f, -2.0f),
    10.0f,
    std::make_unique<LambertianMaterial>(glm::vec3(1.0f, 0.28f, 0.5f))
  );
  HitableList hitables;

  // TODO move to InputSystem class
  // Keyboard
  bool wDown = false;
  bool aDown = false;
  bool sDown = false;
  bool dDown = false;
  // Mouse
  bool rmbDown = false;
  int x = 0;
  int y = 0;
  int lastX = 0;
  int lastY = 0;

  void Init();
  void Update(float deltaTime);
  void Render();
  glm::vec3 Color(const Ray& ray, int depth);
};
