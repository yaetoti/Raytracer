#pragma once

#include "MainScene.h"
#include "Flame.h"
#include "ConsoleLib.h"

#include <chrono>
#include <random>
#include <thread>
#include <Windows.h>

struct Application final : Flame::EventListener<Flame::WindowEvent> {
  Application();

  void Run();
  void HandleEvent(const Flame::WindowEvent& e) override;

  float GetTime() const;
  float GetDeltaTime() const;

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

  std::shared_ptr<Flame::SceneOld> m_scene;
  std::shared_ptr<Flame::AlignedCamera> m_camera;
  std::shared_ptr<Flame::Renderer> m_renderer;

  std::shared_ptr<Flame::MeshSystem> m_meshSystem;
  std::unique_ptr<Flame::DxRenderer> m_dxRenderer;

  uint32_t m_frames = 0;
  uint32_t m_lastFps = 0;
  float m_fpsTimer = 0.0f;
  float m_time = 0.0f;
  float m_deltaTime = 0.0f;

  std::unique_ptr<Flame::IDragger> m_dragger;
};
