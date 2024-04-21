#pragma once

#include <chrono>
#include <ConsoleLib.h>
#include <Flame.h>
#include <random>
#include <thread>
#include <Windows.h>

#include "Flame/LayerStack.h"

struct Application final : EventListener<WindowEvent> {
  Application();

  void Run();
  void HandleEvent(const WindowEvent& e) override;

private:
  void Init();
  void Update(float deltaTime);
  void Render();
  glm::vec3 Color(const Ray& ray, int depth);

private:
  Flame::LayerStack m_layerStack;

  std::shared_ptr<Window> m_window;
  InputSystem* m_input;
  RenderSurface m_framebuffer;

  std::vector<std::unique_ptr<IHitable>> m_hitables;
  std::vector<std::unique_ptr<IMaterial>> m_materials;
  HitableList m_hitableList;

  Sphere* m_sphere2;
  Sphere* m_grabbed = nullptr;
  float m_grabbedTime = 0.0f;
  glm::vec3 m_grabbedOffset;
};
