#pragma once

#include "Flame/layers/LayerStack.h"
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
  Flame::LayerStack m_layerStack;

  std::shared_ptr<Flame::Window> m_window;
  Flame::InputSystem* m_input;
  Flame::RenderSurface m_framebuffer;
};
