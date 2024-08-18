#include "Application.h"
#include "Flame/engine/LightSystem.h"
#include "Flame/engine/ModelManager.h"
#include "Flame/engine/TextureManager.h"
#include "Flame/engine/Transform.h"
#include "Flame/engine/lights/PointLight.h"
#include "Flame/engine/lights/SpotLight.h"
#include "glm/trigonometric.hpp"
#include <Flame/engine/MeshSystem.h>
#include <cmath>

Application::Application() {
  m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 160, 90, 1);
  m_input = &m_window->GetInputSystem();
  m_camera = std::make_shared<Flame::AlignedCamera>(m_window->GetWidth(), m_window->GetHeight(), 60.0f, 0.01f, 1000.0f);
  m_camera->SetPosition(glm::vec3(0, 0, 2));
  m_meshSystem = std::make_shared<Flame::MeshSystem>();
  m_dxRenderer = std::make_unique<Flame::DxRenderer>(m_window, m_camera);
  m_dragger = nullptr;
}

void Application::Run() {
  Init();

  // FPS limiter
  constexpr float targetFps = 60.0f;
  constexpr float targetDeltaTime = 1.0f / targetFps;

  // Main loop
  MSG message;
  Flame::Timer timer;
  while (true) {
    m_deltaTime = std::max(timer.Tick(), targetDeltaTime);
    m_time += m_deltaTime;
    CountFps(m_deltaTime);

    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        return;
      }

      TranslateMessage(&message);
      DispatchMessageW(&message);
    }

    Update(m_deltaTime);
    Render();

    while (timer.GetTimeSinceTick() < targetDeltaTime) {
      std::this_thread::yield();
    }
  }
}

void Application::Init() {
  m_dxRenderer->Init();
  m_window->GetDispatcher().AddListener(this);
  m_window->CreateResources();
  m_window->Show(SW_SHOW);

  // Init scene
  using Transform = Flame::Transform;
  using MathUtils = Flame::MathUtils;
  using DirectLight = Flame::DirectLight;
  using PointLight = Flame::PointLight;
  using SpotLight = Flame::SpotLight;

  Flame::MeshSystem* ms = Flame::MeshSystem::Get();
  Flame::TransformSystem* ts = Flame::TransformSystem::Get();
  Flame::ModelManager* mm = Flame::ModelManager::Get();
  Flame::TextureManager* tm = Flame::TextureManager::Get();
  Flame::LightSystem* ls = Flame::LightSystem::Get();

  // Opaque group
  {
    auto* group = Flame::MeshSystem::Get()->GetOpaqueGroup();

    auto modelId0 = group->AddModel(mm->GetModel("Assets/Models/Samurai/Samurai1.obj"));
    auto model0 = group->GetModel(modelId0);
    auto material1 = model0->AddMaterial({});
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(-2, -2, 0)) }) });
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(-2, 2, 0)) }) });
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(2, -2, 0)) }) });
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(2, 2, 0)) }) });
    auto material2 = model0->AddMaterial({});
    material2->AddInstance({ ts->Insert({ Transform(glm::vec3(0, 0, 2)) }) });
    material2->AddInstance({ ts->Insert({ Transform(glm::vec3(0, 0, -2)) }) });

    auto modelId1 = group->AddModel(mm->GetModel("Assets/Cube.obj"));
    auto model1 = group->GetModel(modelId1);
    auto material3 = model1->AddMaterial({});
    material3->AddInstance({ ts->Insert({ Transform(glm::vec3(-2, -2, 2), glm::vec3(0.5f)) }) });
    material3->AddInstance({ ts->Insert({ Transform(glm::vec3(-2, 2, -2)) }) });
    material3->AddInstance({ ts->Insert({ Transform(glm::vec3(2, -2, -2), glm::vec3(0.1f)) }) });
    material3->AddInstance({ ts->Insert({ Transform(glm::vec3(2, 2, 2)) }) });
  }
  
  // Hologram group
  {
    auto* group = ms->GetHologramGroup();

    auto modelId0 = group->AddModel(mm->GetModel("Assets/Models/Samurai/Samurai1.obj"));
    auto model0 = group->GetModel(modelId0);
    auto material0 = model0->AddMaterial({});
    material0->AddInstance({ ts->Insert({ Transform(glm::vec3(0.0f), glm::vec3(0.5f)) }), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0) });
    material0->AddInstance({ ts->Insert({ Transform(glm::vec3(1.25f, 0.0f, 0.0f)) }), glm::vec3(0, 1, 0), glm::vec3(0, 1, 1) });
    
    auto modelId1 = group->AddModel(mm->GetModel("Assets/Cube.obj"));
    auto model1 = group->GetModel(modelId1);
    auto material1 = model1->AddMaterial({});
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(0.0f, -8.0f, 0.0f), glm::vec3(2.5f)) }), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1) });
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(-8.0f, 0.0f, 0.0f), glm::vec3(3.5f)) }), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) });
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(0.0f, 0.0f, -8.0f), glm::vec3(1.5f)) }), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) });
  }

  // TextureOnly group
  {
    auto* group = ms->GetTextureOnlyGroup();

    auto modelId0 = group->AddModel(mm->GetModel("Assets/Models/OtherCube/OtherCube.obj"));
    auto model0 = group->GetModel(modelId0);
    auto material0 = model0->AddMaterial({ tm->GetTexture(L"Assets/Models/OtherCube/OtherCube.dds")->GetResourceView() });
    material0->AddInstance({ ts->Insert({ Transform(glm::vec3(0.0f, 6.0f, 20.0f), glm::vec3(10.0f)) }) });
    auto material1 = model0->AddMaterial({ tm->GetTexture(L"Assets/Models/OtherCube/AnotherCube.dds")->GetResourceView() });
    material1->AddInstance({ ts->Insert({ Transform(glm::vec3(3.0f, 7.0f, 3.0f), glm::vec3(1.5f)) }) });
  }

  // EmissionOnly group and PointLights
  {
    auto* group = ms->GetEmissionOnlyGroup();

    auto transformId = ts->Insert({ Transform(glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(0.1f)) });

    auto modelId0 = group->AddModel(mm->GetBuiltinModel(Flame::ModelManager::BuiltinModelType::UNIT_SPHERE));
    auto model0 = group->GetModel(modelId0);
    auto material0 = model0->AddMaterial({});
    material0->AddInstance({ transformId, MathUtils::ColorFromHex(0xf194ff) });

    ls->AddPointLight(std::make_shared<PointLight>(
      transformId,
      glm::vec3(0, 0, 0),
      MathUtils::ColorFromHex(0xf194ff),
      4.0f,
      0.0f, 1.2f, 0.18f
    ));
  }

  // TODO 2-step initialization is kinda bad because you can forget about that. Don't know what to to at the moment
  ms->GetOpaqueGroup()->InitInstanceBuffer();
  ms->GetHologramGroup()->InitInstanceBuffer();
  ms->GetTextureOnlyGroup()->InitInstanceBuffer();
  ms->GetEmissionOnlyGroup()->InitInstanceBuffer();

  // Init lights
  ls->AddDirectLight(std::make_shared<DirectLight>(
    MathUtils::ColorFromHex(0x000000),
    glm::vec3(0, -1, 0),
    1.0f
  ));

  m_flashlightId = ls->AddSpotLight(std::make_shared<SpotLight>(
    glm::vec3(0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    MathUtils::ColorFromHex(0xFFFFFF),
    1.5f,
    glm::cos(glm::radians(20.0f)),
    glm::cos(glm::radians(25.0f)),
    1.0f, 0.045f, 0.0075f
  ));
}

void Application::Update(float deltaTime) {
  if (m_input->IsKeyPressed(VK_ESCAPE)) {
    PostQuitMessage(0);
  }

  UpdateCamera(deltaTime);
  UpdateGrabbing(deltaTime);
  m_dxRenderer->Update(deltaTime);

  // Input update must take place at the end to properly update last cursor coordinates TODO Fix
  m_input->Update();

  // Update flashlight
  if (m_flashlightGrabbed) {
    Flame::LightSystem* ls = Flame::LightSystem::Get();
    auto flashlight = ls->GetSpotLight(m_flashlightId);
    flashlight->position = m_camera->GetPosition();
    flashlight->direction = m_camera->GetFrontUnit();
  }
}

void Application::Render() const {
  m_dxRenderer->Render(m_time, m_deltaTime);
  m_window->PresentSwapchain();
}

void Application::HandleEvent(const Flame::WindowEvent& e) {
  if (e.type == Flame::WindowEventType::RESIZE) {
    m_camera->Resize(m_window->GetFramebuffer().GetWidth(), m_window->GetFramebuffer().GetHeight());
    m_dxRenderer->Resize(m_window->GetFramebuffer().GetWidth(), m_window->GetFramebuffer().GetHeight());
    return;
  }
  if (e.type == Flame::WindowEventType::KEY) {
    const auto& event = static_cast<const Flame::KeyWindowEvent&>(e);
    if (event.vkCode == 'N' && event.isPressed && !event.wasPressed) {
      m_dxRenderer->SetNormalVisMode(!m_dxRenderer->GetNormalVisMode());
    }

    if (event.vkCode == 'F' && event.isPressed && !event.wasPressed) {
      m_flashlightGrabbed = !m_flashlightGrabbed;
    }
  }
}

float Application::GetTime() const {
  return m_time;
}

float Application::GetDeltaTime() const {
  return m_deltaTime;
}

void Application::UpdateCamera(float deltaTime) {
  // TODO Move into CameraController
  static float baseSpeed = 5.0f;
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
  if (m_input->IsKeyPressed('Q')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * -speed * deltaTime);
  }
  if (m_input->IsKeyPressed('E')) {
    m_camera->SetPosition(m_camera->GetPosition() + m_camera->GetUpUnit() * speed * deltaTime);
  }

  // Rotation
  if (m_input->IsMouseButtonPressed(Flame::MouseButton::LEFT)) {
    static float rotationSpeedDeg = -180.0f;
    constexpr float sensitivity = 1.0f;
    auto[x, y] = m_input->GetCursorPos();
    float width = m_window->GetWidth();
    float height = m_window->GetHeight();
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;

#if 1
    float deltaX = (x - halfWidth) / width * sensitivity;
    float deltaY = (y - halfHeight) / height * sensitivity;
    m_camera->Rotate(deltaY * rotationSpeedDeg * deltaTime, deltaX * rotationSpeedDeg * deltaTime);
#else
    auto[lastX, lastY] = m_input->GetLastCursorPos();
    float deltaX = ((x - lastX) / width) * sensitivity;
    float deltaY = ((y - lastY) / height) * sensitivity;
    m_camera->Rotate(deltaY * rotationSpeedDeg, deltaX * rotationSpeedDeg);
#endif
  }

  // Grabbing
  UpdateGrabbing(deltaTime);
}

void Application::UpdateGrabbing(float deltaTime) {
  if (m_input->IsMouseButtonPressed(Flame::MouseButton::RIGHT)) {
    auto[x, y] = m_input->GetCursorPos();
    Flame::Ray ray = m_camera->GetRay(static_cast<uint32_t>(x), static_cast<uint32_t>(y));

    if (!m_dragger) {
      Flame::HitRecord<Flame::MeshSystem::HitResult> record;
      if (Flame::MeshSystem::Get()->Hit(ray, record, 0.0f, 1000.0f)) {
        m_dragger = Flame::DraggerFactory::CreateDragger(record, m_camera->GetPosition(), m_camera->GetFrontUnit());
      }
    } else {
      m_dragger->Drag(ray, m_camera->GetFrontUnit());
    }
  } else {
    if (m_dragger != nullptr) {
      m_dragger = nullptr;
    }
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
