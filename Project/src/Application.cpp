#include "Application.h"

#include "Flame/math/MeshData.h"
#include "Flame/math/Mesh.h"
#include "Flame/utils/ObjUtils.h"

Application::Application() {
  // m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 1366, 768, 2);
  // High performance mode
  m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 160, 90, 1);
  m_input = &m_window->GetInputSystem();
  // TODO Pass width and height = mistake
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
    // TODO Do not cuff camera to a resolution
    m_camera->Resize(m_window->GetFramebuffer().GetWidth(), m_window->GetFramebuffer().GetHeight());
    return;
  }
}

void Application::UpdateCamera(float deltaTime) {
  // TODO Move into CameraController
  bool moved = false;
  float speed = 4.0f;
  float rollSpeed = glm::radians(45.0f);
  // TODO ScrollWheel

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
    m_camera->Rotate(glm::eulerAngleZ(rollSpeed * deltaTime));
    moved = true;
  }
  if (m_input->IsKeyPressed('E')) {
    m_camera->Rotate(glm::eulerAngleZ(-rollSpeed * deltaTime));
    moved = true;
  }
  // Rotation
  if (m_input->IsMouseButtonPressed(Flame::MouseButton::LEFT)) {
    static float rotationSpeed = -glm::pi<float>();
    constexpr float sensitivity = 1.0f;
    auto[x, y] = m_input->GetCursorPos();
    auto[lastX, lastY] = m_input->GetLastCursorPos();
    float deltaX = ((x - lastX) / m_window->GetWidth()) * sensitivity;
    float deltaY = ((y - lastY) / m_window->GetHeight()) * sensitivity;
    m_camera->Rotate(glm::eulerAngleY(deltaX * rotationSpeed));
    m_camera->Rotate(glm::eulerAngleX(deltaY * rotationSpeed));
    moved = true;
  }

  if (moved) {
    m_scene->ResetAccumulatedData();
  }
}

void Application::UpdateGrabbing(float deltaTime) {
  bool moved = false;
  static Flame::Sphere* grabbed = nullptr;
  static float grabbedTime = 0.0f;
  static glm::vec3 grabbedOffset;
  auto[x, y] = m_input->GetCursorPos();
  // TODO Do something with that
  x /= m_window->GetResolutionDivisor();
  y /= m_window->GetResolutionDivisor();
  // TODO Fix window inversion
  y = m_window->GetFramebuffer().GetHeight() - y;

  if (m_input->IsMouseButtonPressed(Flame::MouseButton::RIGHT)) {
    if (grabbed == nullptr) {
      Flame::HitRecord record;
      std::vector<std::unique_ptr<Flame::IHitable>>& hitables = m_scene->GetHitables();

      if (Flame::MathUtils::HitClosest(hitables.begin(), hitables.end(), m_camera->GetRay(x, y), 0.0f, 1000.0f, record)) {
        if (auto sphere = dynamic_cast<Flame::Sphere*>(record.hitable)) {
          grabbed = sphere;
          grabbedTime = record.time;
          grabbedOffset = sphere->center - record.point;
        }
      }
    }
  } else {
    grabbed = nullptr;
  }

  if (grabbed != nullptr) {
    grabbed->center = m_camera->GetRay(x, y).AtParameter(grabbedTime) + grabbedOffset;
    moved = true;
  }

  if (moved) {
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
