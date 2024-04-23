#include "Application.h"

Application::Application() {
  m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 1366, 768);
  m_input = &m_window->GetInput();
  // TODO MoveIntoWindow
  m_framebuffer = std::make_shared<Flame::RenderSurface>(m_window->GetWidth(), m_window->GetHeight());
  // TODO Pass width and height = mistake
  m_camera = std::make_shared<Flame::Camera>(m_framebuffer->GetWidth(), m_framebuffer->GetHeight(), 90.0f, 0.1f, 1000.0f);
}

void Application::Run() {
  // TODO Create Engine::Logger
  Console::GetInstance()->RedirectStdHandles();
  Init();

  // FPS limiter
  constexpr float targetFps = 60.0f;
  constexpr float targetDeltaTime = 1.0f / targetFps;
  // FPS counter
  int frames = 0;
  int lastFps = 0;
  float elapsedFpsCounter = 0.0;

  // Main loop
  MSG message;
  Flame::Timer timer;
  while (true) {
    float deltaTime = std::max(timer.Tick(), targetDeltaTime);

    // TODO Move somewhere
    elapsedFpsCounter += deltaTime;
    if (elapsedFpsCounter >= 1.0) {
      elapsedFpsCounter = 0.0;
      lastFps = frames;
      frames = 0;
      std::cout << "FPS: " << lastFps << '\n';
    }
    ++frames;

    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        return;
      }

      TranslateMessage(&message);
      DispatchMessageW(&message);
    }

    Update(deltaTime);
    Render();
    // TODO Scene->Render()
    m_window->Blit(*m_framebuffer);

    while (timer.GetTimeSinceTick() < targetDeltaTime) {
      std::this_thread::yield();
    }
  }
}

void Application::Init() {
  m_window->GetDispatcher()->AddListener(this);
  m_window->CreateResources();
  m_window->Show(SW_SHOW);

  m_scene = std::make_shared<MainScene>(*m_window);
  m_scene->Initialize();
}

void Application::Update(float deltaTime) {
  if (m_input->IsKeyPressed(VK_ESCAPE)) {
    PostQuitMessage(0);
  }

  // HandleCameraUpdate
  static float speed = 4.0f;
  static float rollSpeed = glm::radians(45.0f);
  static float rotationSpeed = -glm::radians(1.0f);
  // TODO ScrollWheel

  if (m_input->IsKeyPressed('A')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetRightUnit() * -speed * deltaTime);
  }
  if (m_input->IsKeyPressed('D')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetRightUnit() * speed * deltaTime);
  }
  if (m_input->IsKeyPressed('W')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetFrontUnit() * speed * deltaTime);
  }
  if (m_input->IsKeyPressed('S')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetFrontUnit() * -speed * deltaTime);
  }
  if (m_input->IsKeyPressed(VK_SPACE)) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * speed * deltaTime);
  }
  if (m_input->IsKeyPressed(VK_CONTROL)) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * -speed * deltaTime);
  }
  // DO A BARREL ROLL
  if (m_input->IsKeyPressed('Q')) {
    m_camera->Rotate(glm::eulerAngleZ(rollSpeed * deltaTime));
  }
  if (m_input->IsKeyPressed('E')) {
    m_camera->Rotate(glm::eulerAngleZ(-rollSpeed * deltaTime));
  }
  // Rotation
  if (m_input->IsMouseButtonPressed(Flame::MouseButton::RIGHT)) {
    auto[x, y] = m_input->GetCursorPos();
    auto[lastX, lastY] = m_input->GetLastCursorPos();
    float deltaX = x - lastX;
    float deltaY = y - lastY;
    m_camera->Rotate(glm::eulerAngleY(deltaX * rotationSpeed * deltaTime));
    m_camera->Rotate(glm::eulerAngleX(deltaY * rotationSpeed * deltaTime));
  }

  // TODO Grabbing
  {
    static Flame::Sphere* grabbed = nullptr;
    static float grabbedTime = 0.0f;
    static glm::vec3 grabbedOffset;
    auto[x, y] = m_input->GetCursorPos();
    // TODO Add input mapping
    x /= 2;
    y /= 2;
    // TODO Fix window inversion
    y = m_framebuffer->GetHeight() - y;

    if (m_input->IsMouseButtonPressed(Flame::MouseButton::LEFT)) {
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
    }
  }

  // TODO Remove
  m_camera->CalculateRays();
  m_scene->Update(deltaTime);

  m_input->Update();
}

void Application::Render() {
  m_scene->Render(*m_framebuffer, *m_camera);
}

void Application::HandleEvent(const Flame::WindowEvent& e) {
  switch (e.type) {
  case Flame::WindowEventType::RESIZE: {
    auto evt = dynamic_cast<const Flame::ResizeWindowEvent*>(&e);
    uint32_t width = evt->width / 2;
    uint32_t height = evt->height / 2;
    m_framebuffer->Resize(width, height);
    m_camera->Resize(width, height);
    return;
  }
  default:
    break;
  }
}
