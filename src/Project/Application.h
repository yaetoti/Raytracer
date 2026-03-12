#pragma once

#include "MainScene.h"

#include <chrono>
#include <ConsoleLib.h>
#include <Flame.h>
#include <random>
#include <thread>
#include <Windows.h>

#include "Flame/render/Camera.h"
#include "Flame/render/Renderer.h"
#include "Flame/utils/draggers/IDragger.h"

struct Application final : Flame::EventListener<Flame::WindowEvent> {
  Application();

  void Run();
  void HandleEvent(const Flame::WindowEvent& e) override;

private:
  void Init();
  void Update(float deltaTime);
  void Render();

  void UpdateCamera(float deltaTime);
  void UpdateGrabbing(float deltaTime);
  void CountFps(float deltaTime);

private:
  std::shared_ptr<Flame::Window> m_window;
  Flame::InputSystem* m_input;

  std::shared_ptr<Flame::Scene> m_scene;
  std::shared_ptr<Flame::Camera> m_camera;
  std::shared_ptr<Flame::Renderer> m_renderer;

  uint32_t m_frames = 0;
  uint32_t m_lastFps = 0;
  float m_fpsTimer = 0.0f;

  std::unique_ptr<Flame::IDragger> m_dragger;
};
