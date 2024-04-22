#pragma once

#include "MainScene.h"

#include <chrono>
#include <ConsoleLib.h>
#include <Flame.h>
#include <random>
#include <thread>
#include <Windows.h>

struct Application final : Flame::EventListener<Flame::WindowEvent> {
  Application();

  void Run();
  void HandleEvent(const Flame::WindowEvent& e) override;

private:
  void Init();
  void Update(float deltaTime);
  void Render();

private:
  std::shared_ptr<Flame::Scene> m_scene;
  std::shared_ptr<Flame::Camera> m_camera;

  std::shared_ptr<Flame::Window> m_window;
  Flame::InputSystem* m_input;
  std::shared_ptr<Flame::RenderSurface> m_framebuffer;
};
