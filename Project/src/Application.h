#pragma once

#include <chrono>
#include <ConsoleLib.h>
#include <Flame.h>
#include <random>
#include <thread>
#include <Windows.h>

struct Application final : EventListener<WindowEvent> {
  Application();

  int RunMainLoop();
  void Handle(const WindowEvent* e) override;

private:
  std::shared_ptr<Window> m_window;
  Framebuffer framebuffer;

  std::mt19937 gen;
  std::uniform_real_distribution<float> dis;

  std::shared_ptr<Sphere> sphere1 = std::make_shared<Sphere>(
    Vec3F(0, 0, -2),
    0.5f,
    std::make_unique<MetalMaterial>(
      Vec3F(0.98f, 0.98f, 0.98f)
    )
  );
  std::shared_ptr<Sphere> sphere2 = std::make_shared<Sphere>(
    Vec3F(0.8f, 0.2f, -1.0f),
    0.4f,
    std::make_unique<AlbedoMaterial>(
      Vec3F(0.28f, 1.0f, 0.5f)
    )
  );
  std::shared_ptr<Sphere> sphere3 = std::make_shared<Sphere>(
    Vec3F(0, -10.5f, -2.0f),
    10.0f,
    std::make_unique<LambertianMaterial>(
      Vec3F(1.0f, 0.28f, 0.5f),
      dis,
      gen
    )
  );
  HitableList hitables;

  // TODO move to InputSystem class
  // Keyboard
  bool wDown = false;
  bool aDown = false;
  bool sDown = false;
  bool dDown = false;
  // Mouse
  bool rmbDown = false;
  int x = 0;
  int y = 0;
  int lastX = 0;
  int lastY = 0;

  void Init();
  void Update(float deltaTime);
  void Render();
  Vec3F Color(const Ray& ray, int depth);
};

inline Application::Application()
: m_window(std::make_shared<Window>(L"Flame 🔥", 1366, 768))
, framebuffer(1366, 768)
, gen(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count())) {
}

inline Vec3F Application::Color(const Ray& ray, int depth) {
  HitRecord record;
  if (hitables.Hit(ray, 0.01f, std::numeric_limits<float>::max(), record)) {
    Ray scattered;
    Vec3F attenuation;

    if (depth < 10 && record.material->Scatter(ray, record, scattered, attenuation)) {
      return attenuation * Color(scattered, depth + 1);
    }

    return Vec3F(0.0f);
  }

  Vec3F skyColor(0.5f, 0.7f, 1.0f);
  float t = (ray.direction.y + 1) * 0.5f;
  return skyColor * t + (1.0f - t) * Vec3F(1.0f);
}

inline void Application::Render() {
  int rays = 5;
  float raysScale = 1.0f / rays;

  int width = static_cast<int>(framebuffer.GetWidth());
  int height = static_cast<int>(framebuffer.GetHeight());
  float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
  float scaleX = std::min(aspectRatio, 1.0f);
  float scaleY = std::min(1 / aspectRatio, 1.0f);
  Vec3F unit(1.0f / width, 1.0f / height, 0);

  for (int col = 0; col < height; ++col) {
    for (int row = 0; row < width; ++row) {
      // TODO Camera->GetRay(u, v)
      float x = (2.0f * row / width - 1.0f) * scaleX;
      float y = (2.0f * col / height - 1.0f) * scaleY;

      Vec3F resultColor;
      for (int i = 0; i < rays; ++i) {
        Ray ray(Vec3F(x, y, 0) + unit * dis(gen), Vec3F(0, 0, -1.0));
        resultColor += Color(ray, 0);
      }

      resultColor *= raysScale;
      resultColor *= 255.0f;
      framebuffer.SetPixel(row, col, static_cast<BYTE>(resultColor.r), static_cast<BYTE>(resultColor.g), static_cast<BYTE>(resultColor.b));
    }
  }
}

inline int Application::RunMainLoop() {
  Init();

  MSG message;
  Timer timer;

  // FPS limiter
  int targetFps = 120;
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
        return static_cast<int>(message.wParam);
      }

      TranslateMessage(&message);
      DispatchMessageW(&message);
    }

    // Update
    ++frames;

    Update(deltaTime);
    Render();
    // TODO Scene->Render()
    m_window->Blit(framebuffer);

    // Sleep
    while (Timer::Duration(Timer::Clock::now() - timer.GetCurrentTimePoint()).count() < targetDeltaTime) {
      std::this_thread::yield();
    }
  }
}

inline void Application::Handle(const WindowEvent* e) {
  switch (e->type) {
  case WindowEventType::RESIZE: {
    auto evt = dynamic_cast<const ResizeWindowEvent*>(e);
    framebuffer.Resize(evt->width / 4, evt->height / 4);
    break;
  }
  case WindowEventType::KEY: {
    auto evt = dynamic_cast<const KeyWindowEvent*>(e);
    if (evt->vkCode == 'A') {
      aDown = evt->isPressed;
      return;
    }
    if (evt->vkCode == 'D') {
      dDown = evt->isPressed;
      return;
    }
    if (evt->vkCode == 'W') {
      wDown = evt->isPressed;
      return;
    }
    if (evt->vkCode == 'S') {
      sDown = evt->isPressed;
      return;
    }
    break;
  }
  case WindowEventType::MOUSE_BUTTON: {
    auto evt = dynamic_cast<const MouseButtonWindowEvent*>(e);
    if (!evt->isLeft) {
      rmbDown = evt->isPressed;
    }
    break;
  }
  case WindowEventType::MOUSE_MOVE: {
    auto evt = dynamic_cast<const MouseMoveWindowEvent*>(e);
    lastX = x;
    lastY = y;
    x = evt->deltaX;
    y = evt->deltaY;
    break;
  }
  default:
    break;
  }
}

inline void Application::Init() {
  hitables.Add(sphere1);
  hitables.Add(sphere2);
  hitables.Add(sphere3);

  m_window->GetDispatcher()->AddListener(this);
  m_window->CreateResources();
  m_window->Show(SW_SHOW);
}

inline void Application::Update(float deltaTime) {
  if (aDown) {
    sphere2->center -= Vec3F(1.0f * deltaTime, 0.0f, 0.0f);
  }
  if (dDown) {
    sphere2->center -= Vec3F(-1.0f * deltaTime, 0.0f, 0.0f);
  }
  if (wDown) {
    sphere2->center -= Vec3F(0, -1.0f * deltaTime, 0.0f);
  }
  if (sDown) {
    sphere2->center -= Vec3F(0, 1.0f * deltaTime, 0.0f);
  }

  if (rmbDown) {
    sphere2->center -= Vec3F((lastX - x) * deltaTime, -(lastY - y) * deltaTime, 0.0f);
    lastX = x;
    lastY = y;
  }
}
