#include "Application.h"

Application::Application()
: m_window(std::make_shared<Window>(L"Flame 🔥", 1366, 768))
, m_input(&m_window->GetInput())
, m_framebuffer(1366, 768) {
}

 glm::vec3 Application::Color(const Ray& ray, int depth) {
  HitRecord record;
  if (m_hitableList.Hit(ray, 0.01f, std::numeric_limits<float>::max(), record)) {
    Ray scattered;
    glm::vec3 attenuation;

    if (depth < 10 && record.material->Scatter(ray, record, scattered, attenuation)) {
      return attenuation * Color(scattered, depth + 1);
    }

    return glm::vec3(0.0f);
  }

  glm::vec3 skyColor(0.5f, 0.7f, 1.0f);
  float t = (ray.direction.y + 1) * 0.5f;
  return skyColor * t + (1.0f - t) * glm::vec3(1.0f);
}

void Application::Run() {
  // TODO Create Engine::Logger
  Console::GetInstance()->RedirectStdHandles();
  Init();

  MSG message;
  Timer timer;

  // FPS limiter
  int targetFps = 60;
  float targetDeltaTime = 1.0f / targetFps;
  float deltaTime = 0.0;
  // FPS counter
  int frames = 0;
  int lastFps = 0;
  float fpsElapsed = 0.0;

  while (true) {
    deltaTime = std::max(timer.Tick(), targetDeltaTime);

    // FPS counter
    fpsElapsed += deltaTime;
    if (fpsElapsed >= 1.0) {
      fpsElapsed = 0.0;
      lastFps = frames;
      frames = 0;
      Console::GetInstance()->WPrintF(L"FPS: %d\n", lastFps);
    }

    // Input
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        return;
      }

      TranslateMessage(&message);
      DispatchMessageW(&message);
    }

    // Update
    ++frames;

    Update(deltaTime);
    Render();
    // TODO Scene->Render()
    m_window->Blit(m_framebuffer);

    // Sleep
    while (Timer::Duration(Timer::Clock::now() - timer.GetCurrentTimePoint()).count() < targetDeltaTime) {
      std::this_thread::yield();
    }
  }
}

void Application::Init() {
  // Create materials
  m_materials.emplace_back(
   std::make_unique<MetalMaterial>(glm::vec3(0.98f, 0.98f, 0.98f))
  );
  m_materials.emplace_back(
   std::make_unique<AlbedoMaterial>(glm::vec3(0.28f, 1.0f, 0.5f))
  );
  m_materials.emplace_back(
   std::make_unique<LambertianMaterial>(glm::vec3(1.0f, 0.28f, 0.5f))
  );

  // Create spheres
  m_hitables.emplace_back(
   std::make_unique<Sphere>(
     glm::vec3(0, 0, -2),
     0.5f,
     *m_materials[0]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Sphere>(
     glm::vec3(0, 1, -1),
     0.3f,
     *m_materials[1]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Sphere>(
     glm::vec3(0, -50.5, -1),
     50.0f,
     *m_materials[2]
   )
  );

  for (auto& hitable : m_hitables) {
    m_hitableList.Add(hitable.get());
  }

  m_sphere2 = dynamic_cast<Sphere*>(m_hitables[1].get());


  m_window->GetDispatcher()->AddListener(this);
  m_window->CreateResources();
  m_window->Show(SW_SHOW);
}

void Application::Update(float deltaTime) {
  if (m_window->GetInput().IsKeyPressed('A')) {
    m_sphere2->center -= glm::vec3(1.0f * deltaTime, 0.0f, 0.0f);
  }
  if (m_window->GetInput().IsKeyPressed('D')) {
    m_sphere2->center -= glm::vec3(-1.0f * deltaTime, 0.0f, 0.0f);
  }
  if (m_window->GetInput().IsKeyPressed('W')) {
    m_sphere2->center -= glm::vec3(0, -1.0f * deltaTime, 0.0f);
  }
  if (m_window->GetInput().IsKeyPressed('S')) {
    m_sphere2->center -= glm::vec3(0, 1.0f * deltaTime, 0.0f);
  }

  if (m_window->GetInput().IsMouseButtonPressed(MouseButton::RIGHT)) {
    auto[x, y] = m_input->GetCursorPos();
    auto[lastX, lastY] = m_input->GetLastCursorPos();
    m_sphere2->center -= glm::vec3((lastX - x) * deltaTime, -(lastY - y) * deltaTime, 0.0f);
  }

  // TODO Grabbing
  
  auto[x, y] = m_input->GetCursorPos();
  glm::mat4 perspective = glm::ortho(
    0.0f,
    static_cast<float>(m_window->GetWidth()),
    static_cast<float>(m_window->GetHeight()),
    0.0f);
  glm::mat4 inversed = glm::inverse(perspective);
  glm::vec4 pos(x, y, 0.0f, 1.0f);
  std::cout << pos << " " << perspective * pos << '\n';

  if (m_input->IsMouseButtonPressed(MouseButton::LEFT)) {
    if (m_grabbed == nullptr) {
      Ray ray(glm::vec3(perspective * pos), glm::vec3(0.0f, 0.0f, -1.0f));
      HitRecord record;
      if (m_sphere2->Hit(ray, 0.0f, 1000.0f, record)) {
        m_grabbed = m_sphere2;
        m_grabbedTime = record.time;
        m_grabbedOffset = m_sphere2->center - record.point;
      }
    }
  } else {
    m_grabbed = nullptr;
  }

  if (m_grabbed != nullptr) {
    m_grabbed->center = glm::vec3(perspective * pos) + glm::vec3(0.0f, 0.0f, -1.0f) * m_grabbedTime + m_grabbedOffset;
  }

  m_input->Update();
}

void Application::Render() {
  int rays = 3;
  float raysScale = 1.0f / rays;

  int width = static_cast<int>(m_framebuffer.GetWidth());
  int height = static_cast<int>(m_framebuffer.GetHeight());
  float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
  float scaleX = std::min(aspectRatio, 1.0f);
  float scaleY = std::min(1 / aspectRatio, 1.0f);
  glm::vec3 unit(1.0f / width, 1.0f / height, 0);

  for (int col = 0; col < height; ++col) {
    for (int row = 0; row < width; ++row) {
      // TODO Camera->GetRay(u, v)
      float x = (2.0f * row / width - 1.0f) * scaleX;
      float y = (2.0f * col / height - 1.0f) * scaleY;

      glm::vec3 resultColor(0);
      for (int i = 0; i < rays; ++i) {
        Ray ray(glm::vec3(x, y, 0) + unit * Random::Float(), glm::vec3(0, 0, -1));
        resultColor += Color(ray, 0);
      }

      resultColor *= raysScale;
      resultColor *= 255.0f;
      m_framebuffer.SetPixel(row, col, static_cast<BYTE>(resultColor.r), static_cast<BYTE>(resultColor.g), static_cast<BYTE>(resultColor.b));
    }
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
