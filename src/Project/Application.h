﻿#pragma once

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
  void Cleanup();
  void Update(float deltaTime);
  void Render() const;

  void UpdateCamera(float deltaTime);
  void UpdateGrabbing(float deltaTime);
  void CountFps(float deltaTime);

private:
  std::shared_ptr<Flame::Window> m_window;
  Flame::InputSystem* m_input;

  std::shared_ptr<Flame::AlignedCamera> m_camera;
  std::shared_ptr<Flame::DxRenderer> m_dxRenderer;

  std::unique_ptr<Flame::IDragger> m_dragger;

  bool m_flashlightGrabbed = true;
  uint32_t m_flashlightId;

  uint32_t m_frames = 0;
  uint32_t m_lastFps = 0;
  float m_fpsTimer = 0.0f;
  float m_time = 0.0f;
  float m_deltaTime = 0.0f;

  // TODO test
  uint32_t m_planeTransformId;
  uint32_t m_cubeTransformId;
  float m_rotation = 0.0f;
};
