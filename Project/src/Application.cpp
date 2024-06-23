#include "Application.h"

#include "Flame/math/MeshData.h"
#include "Flame/math/Mesh.h"
#include "Flame/utils/ObjUtils.h"

Application::Application() {
  // m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 1366, 768, 2);
  // High performance mode
  m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 160, 90, 1);
  m_input = &m_window->GetInputSystem();
  m_camera = std::make_shared<Flame::Camera>(m_window->GetFramebuffer().GetWidth(), m_window->GetFramebuffer().GetHeight(), 90.0f, 0.1f, 1000.0f);
}

void Application::Run() {
  // TODO Create Engine::Logger
  Console::GetInstance()->RedirectStdHandles();
  Init();

  // FPS limiter
  constexpr float targetFps = 60.0f;
  constexpr float targetDeltaTime = 1.0f / targetFps;

  // Main loop
  MSG message;
  Flame::Timer timer;
  while (true) {
    float deltaTime = std::max(timer.Tick(), targetDeltaTime);
    CountFps(deltaTime);

    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        return;
      }

      TranslateMessage(&message);
      DispatchMessageW(&message);
    }

    Update(deltaTime);
    Render();
    m_window->Blit();

    while (timer.GetTimeSinceTick() < targetDeltaTime) {
      std::this_thread::yield();
    }
  }
}

void Application::Init() {
  m_scene = std::make_shared<MainScene>(*m_window);
  m_scene->Initialize();

  m_window->GetDispatcher().AddListener(this);
  m_window->GetDispatcher().AddListener(m_scene.get());

  m_window->CreateResources();
  m_window->Show(SW_SHOW);
}

void Application::Update(float deltaTime) {
  if (m_input->IsKeyPressed(VK_ESCAPE)) {
    PostQuitMessage(0);
  }

  UpdateCamera(deltaTime);
  UpdateGrabbing(deltaTime);
  m_scene->Update(deltaTime);

  // Input update must take place at the end to properly update last cursor coordinates TODO Fix
  m_input->Update();
}

void Application::Render() {
  m_scene->Render(m_window->GetFramebuffer(), *m_camera);
}

void Application::HandleEvent(const Flame::WindowEvent& e) {
  if (e.type == Flame::WindowEventType::RESIZE) {
    m_camera->Resize(m_window->GetFramebuffer().GetWidth(), m_window->GetFramebuffer().GetHeight());
    return;
  }
}

void Application::UpdateCamera(float deltaTime) {
  // TODO Move into CameraController
  static float baseSpeed = 2.0f;
  bool moved = false;
  float speed = baseSpeed;
  float rollSpeedDeg = 90.0f;

  // Movement speed
  if (m_input->IsKeyPressed(VK_SHIFT)) {
    speed *= 5;
  }
  if (m_input->GetScrollDelta() != 0.0f) {
    baseSpeed += baseSpeed * 0.05f * m_input->GetScrollDelta();
  }

  // Movement
  if (m_input->IsKeyPressed('A')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetRightUnit() * -speed * deltaTime);
    moved = true;
  }
  if (m_input->IsKeyPressed('D')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetRightUnit() * speed * deltaTime);
    moved = true;
  }
  if (m_input->IsKeyPressed('W')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetFrontUnit() * speed * deltaTime);
    moved = true;
  }
  if (m_input->IsKeyPressed('S')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetFrontUnit() * -speed * deltaTime);
    moved = true;
  }
  if (m_input->IsKeyPressed(VK_SPACE)) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * speed * deltaTime);
    moved = true;
  }
  if (m_input->IsKeyPressed(VK_CONTROL)) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * -speed * deltaTime);
    moved = true;
  }
  // DO A BARREL ROLL
  if (m_input->IsKeyPressed('Q')) {
    m_camera->Rotate(0.0f, 0.0f, rollSpeedDeg * deltaTime);
    moved = true;
  }
  if (m_input->IsKeyPressed('E')) {
    m_camera->Rotate(0.0f, 0.0f, -rollSpeedDeg * deltaTime);
    moved = true;
  }
  // Rotation
  if (m_input->IsMouseButtonPressed(Flame::MouseButton::LEFT)) {
    static float rotationSpeedDeg = -180.0f;
    constexpr float sensitivity = 1.0f;
    auto[x, y] = m_input->GetCursorPos();
    auto[lastX, lastY] = m_input->GetLastCursorPos();
    float deltaX = ((x - lastX) / m_window->GetWidth()) * sensitivity;
    float deltaY = ((y - lastY) / m_window->GetHeight()) * sensitivity;

    m_camera->Rotate(deltaY * rotationSpeedDeg, deltaX * rotationSpeedDeg, 0.0f);
    moved = true;
  }

  if (moved) {
    m_scene->ResetAccumulatedData();
  }
}

void Application::UpdateGrabbing(float deltaTime) {
  bool sceneChanged = false;
  auto[x, y] = m_input->GetCursorPos();
  // TODO Do something with that
  x /= m_window->GetResolutionDivisor();
  y /= m_window->GetResolutionDivisor();
  // TODO Fix window inversion
  y = m_window->GetFramebuffer().GetHeight() - y;
  Flame::Ray ray = m_camera->GetRay(static_cast<uint32_t>(x), static_cast<uint32_t>(y));

  if (m_input->IsMouseButtonPressed(Flame::MouseButton::RIGHT)) {
    if (m_dragger == nullptr) {
      Flame::HitRecord record;
      std::vector<std::unique_ptr<Flame::IHitable>>& hitables = m_scene->GetHitables();
      if (Flame::MathUtils::HitClosest(hitables.begin(), hitables.end(), ray, 0.0f, 1000.0f, record)) {
        m_dragger = Flame::DraggerFactory::CreateDragger(record);
      }
    }
  } else {
    m_dragger = nullptr;
  }

  if (m_dragger != nullptr) {
    m_dragger->Drag(ray);
    sceneChanged = true;
  }

  if (sceneChanged) {
    m_scene->ResetAccumulatedData();
  }
}

void Application::CountFps(float deltaTime) {
  m_fpsTimer += deltaTime;
  if (m_fpsTimer >= 1.0) {
    m_fpsTimer = 0.0;
    m_lastFps = m_frames;
    m_frames = 0;
    std::cout << "FPS: " << m_lastFps << '\n';
  }
  ++m_frames;
}
