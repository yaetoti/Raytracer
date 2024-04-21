#pragma once

#include "Flame/layers/LayerStack.h"
#include "MainScene.h"

#include <chrono>
#include <ConsoleLib.h>
#include <Flame.h>
#include <random>
#include <thread>
#include <Windows.h>

struct Application final : EventListener<WindowEvent> {
  Application();

  void Run();
  void HandleEvent(const WindowEvent& e) override;

private:
  void Init();
  void Update(float deltaTime);
  void Render();

private:
  Flame::LayerStack m_layerStack;

  std::shared_ptr<Window> m_window;
  InputSystem* m_input;
  RenderSurface m_framebuffer;
};
