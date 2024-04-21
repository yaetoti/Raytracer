#include "Application.h"

Application::Application()
// TODO MoveIntoWindow
: m_framebuffer(1366, 768) {
  m_window = std::make_shared<Window>(L"Flame 🔥", 1366, 768);
  m_input = &m_window->GetInput();
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
  Timer timer;
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
    m_window->Blit(m_framebuffer);

    while (Timer::Duration(Timer::Clock::now() - timer.GetCurrentTimePoint()).count() < targetDeltaTime) {
      std::this_thread::yield();
    }
  }
}

void Application::Init() {
  // TODO Move somewhere
  m_layerStack.PushLayer(std::make_unique<MainScene>(*m_window));

  for (auto& layer : m_layerStack) {
    layer->Initialize();
  }

  m_window->GetDispatcher()->AddListener(this);
  m_window->CreateResources();
  m_window->Show(SW_SHOW);
}

void Application::Update(float deltaTime) {
  for (auto& layer : m_layerStack) {
    layer->Update(deltaTime);
  }

  m_input->Update();
}

void Application::Render() {
  for (auto& layer : m_layerStack) {
    layer->Render(m_framebuffer);
  }
}

void Application::HandleEvent(const WindowEvent& e) {
  switch (e.type) {
  case WindowEventType::RESIZE: {
    auto evt = dynamic_cast<const ResizeWindowEvent*>(&e);
    m_framebuffer.Resize(evt->width / 4, evt->height / 4);
    return;
  }
  default:
    break;
  }
}
