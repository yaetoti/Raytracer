#include "Application.h"

Application::Application() {
  m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 1366, 768);
  m_input = &m_window->GetInput();
  // TODO MoveIntoWindow
  m_framebuffer = std::make_shared<Flame::RenderSurface>(m_window->GetWidth(), m_window->GetHeight());
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
  // TODO Pass width and height = mistake
  m_camera = std::make_shared<Flame::Camera>(m_framebuffer->GetWidth(), m_framebuffer->GetHeight(), 90.0f, 0.1f, 1000.0f);
}

void Application::Update(float deltaTime) {
  static float x = 0.0f;
  x += 1.0f * deltaTime;
  //camera.Rotate(glm::eulerAngleXZ(0.02f, 0.05f));
  m_camera->SetPosition(glm::vec3(0.0f, 0.0f, -glm::sin(x) * 0.5f));

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
    m_framebuffer->Resize(evt->width / 2, evt->height / 2);
    return;
  }
  default:
    break;
  }
}
