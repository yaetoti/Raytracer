#include "Application.h"
#include "Flame/engine/LightSystem.h"
#include "Flame/engine/ModelManager.h"
#include "Flame/engine/TextureManager.h"
#include "Flame/engine/Transform.h"
#include "Flame/engine/lights/PointLight.h"
#include "Flame/engine/lights/SpotLight.h"
#include "Flame/graphics/groups/HologramGroup.h"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include <Flame/engine/MeshSystem.h>
#include <cmath>
#include <winuser.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

Application::Application() {
  m_window = std::make_shared<Flame::Window>(L"Flame 🔥", 160, 90, 1);
  m_input = &m_window->GetInputSystem();
  m_camera = std::make_shared<Flame::AlignedCamera>(m_window->GetWidth(), m_window->GetHeight(), 60.0f, 0.01f, 1000.0f);
  m_camera->SetPosition(glm::vec3(0, 0, 2));
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

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Update(m_deltaTime);
    Render();

    while (timer.GetTimeSinceTick() < targetDeltaTime) {
      std::this_thread::yield();
    }
  }
}

void Application::Init() {
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

    // Statue
    {
      uint32_t transformId = ts->Insert({ Transform(glm::vec3(-2, -2, 0)) });

      uint32_t modelId = group->AddModel(mm->GetModel("Assets/Models/EastTower/EastTower.fbx"));
      auto& model = group->GetModels()[modelId];
      uint32_t materialId;

      materialId = model->GetMeshes()[3]->AddMaterial({
        tm->GetTexture(L"Assets/Models/EastTower/dds/Statue_BaseColor.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/EastTower/dds/Statue_Normal.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/EastTower/dds/Statue_Roughness.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/EastTower/dds/Statue_Metallic.dds")->GetResourceView(),
      });
      auto& material = model->GetMeshes()[3]->GetMaterials()[materialId];
      material->AddInstance({ transformId });
    }

    // Plane
    {
      uint32_t planeTransformId = ts->Insert({ Transform(glm::vec3(0, -6, 0)) });
      m_planeTransformId = planeTransformId;

      group->AddInstance(
        mm->GetModel("Assets/Models/Floor/Floor.fbx"),
        {
          tm->GetTexture(L"Assets/Models/Floor/dds/Albedo.dds")->GetResourceView(),
          tm->GetTexture(L"Assets/Models/Floor/dds/Normal.dds")->GetResourceView(),
          tm->GetTexture(L"Assets/Models/Floor/dds/Roughness.dds")->GetResourceView(),
          tm->GetTexture(L"Assets/Models/Floor/dds/Metallic.dds")->GetResourceView(),
        },
        {
          planeTransformId
        }
      );
    }

    // Cube
    {
      uint32_t cubeTransformId = ts->Insert({ Transform(glm::vec3(-3, 8, 3), glm::vec3(0.01f)) });
      m_cubeTransformId = cubeTransformId;

      uint32_t modelId = group->AddModel(mm->GetModel("Assets/Models/Floor/PbrCube.fbx"));
      auto& model = group->GetModels()[modelId];
      uint32_t materialId;
      uint32_t materialId1;

      materialId = model->GetMeshes()[0]->AddMaterial({
        tm->GetTexture(L"Assets/Models/Floor/dds/Albedo.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/Floor/dds/Normal.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/Floor/dds/Roughness.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/Floor/dds/Metallic.dds")->GetResourceView(),
      });
      auto& material = model->GetMeshes()[0]->GetMaterials()[materialId];
      material->AddInstance({ ts->Insert({ Transform(glm::vec3(0, 6, 0), glm::vec3(0.01f)) }) });
      material->AddInstance({ cubeTransformId });

      materialId1 = model->GetMeshes()[0]->AddMaterial({
        tm->GetTexture(L"Assets/Models/Floor/dds-bathroom/Albedo.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/Floor/dds-bathroom/Normal.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/Floor/dds-bathroom/Roughness.dds")->GetResourceView(),
        tm->GetTexture(L"Assets/Models/Floor/dds-bathroom/Metallic.dds")->GetResourceView(),
      });
      auto& material1 = model->GetMeshes()[0]->GetMaterials()[materialId1];

      material1->AddInstance({ ts->Insert({ Transform(glm::vec3(-3, 6, 0), glm::vec3(0.01f)) }) });
    }
  }
  
  // Hologram group
  {
    auto* group = ms->GetHologramGroup();

    Flame::HologramInstanceData data0[] {
      { ts->Insert({ Transform(glm::vec3(0.0f), glm::vec3(0.5f)) }), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0) },
      { ts->Insert({ Transform(glm::vec3(1.25f, 0.0f, 0.0f)) }), glm::vec3(0, 1, 0), glm::vec3(0, 1, 1) },
    };
    group->AddInstances(mm->GetModel("Assets/Models/Samurai/Samurai1.obj"), {}, data0);
    
    Flame::HologramInstanceData data1[] {
      { ts->Insert({ Transform(glm::vec3(0.0f, -8.0f, 0.0f), glm::vec3(2.5f)) }), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1) },
      { ts->Insert({ Transform(glm::vec3(-8.0f, 0.0f, 0.0f), glm::vec3(3.5f)) }), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) },
      { ts->Insert({ Transform(glm::vec3(0.0f, 0.0f, -8.0f), glm::vec3(1.5f)) }), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) },
    };
    group->AddInstances(mm->GetModel("Assets/Cube.obj"), {}, data1);
  }

  // TextureOnly group
  {
    auto* group = ms->GetTextureOnlyGroup();

    group->AddInstance(
      mm->GetModel("Assets/Models/OtherCube/OtherCube.obj"),
      { tm->GetTexture(L"Assets/Models/OtherCube/OtherCube.dds")->GetResourceView() },
      { ts->Insert({ Transform(glm::vec3(0.0f, 6.0f, 20.0f), glm::vec3(10.0f)) }) }
    );

    group->AddInstance(
      mm->GetModel("Assets/Models/OtherCube/OtherCube.obj"),
      { tm->GetTexture(L"Assets/Models/OtherCube/AnotherCube.dds")->GetResourceView() },
      { ts->Insert({ Transform(glm::vec3(3.0f, 7.0f, 3.0f), glm::vec3(1.5f)) }) }
    );
  }

  // EmissionOnly group and PointLights
  {
    auto* group = ms->GetEmissionOnlyGroup();

    //float radius = 0.1f;
    float radius = 10.0f;
    glm::vec3 radiance = MathUtils::RadianceFromIrradiance(MathUtils::ColorFromHex(0xf194ff), radius, 100.0f);
    auto transformId = ts->Insert({ Transform(glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(radius)) });

    group->AddInstance(
      mm->GetBuiltinModel(Flame::ModelManager::BuiltinModelType::UNIT_SPHERE),
      { },
      { transformId, MathUtils::ColorFromHex(0xf194ff) }
    );

    ls->AddPointLight(std::make_shared<PointLight>(
      transformId,
      glm::vec3(0, 0, 0),
      radiance,
      radius
    ));
  }

  // TODO 2-step initialization is kinda bad because you can forget about that. Don't know what to to at the moment
  ms->GetOpaqueGroup()->InitInstanceBuffer();
  ms->GetHologramGroup()->InitInstanceBuffer();
  ms->GetTextureOnlyGroup()->InitInstanceBuffer();
  ms->GetEmissionOnlyGroup()->InitInstanceBuffer();

  // Init lights
  float sunRadius = 696340.0f;
  float sunDistance = 150000000.0f;
  ls->AddDirectLight(std::make_shared<DirectLight>(
    glm::normalize(glm::vec3(0, -1, 1)),
    MathUtils::RadianceFromIrradiance(MathUtils::ColorFromHex(0x888888), sunRadius, sunDistance),
    MathUtils::SolidAngle(sunRadius, sunDistance)
  ));

  float flashlightRadius = 0.01f;
  m_flashlightId = ls->AddSpotLight(std::make_shared<SpotLight>(
    glm::vec3(0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    MathUtils::RadianceFromIrradiance(MathUtils::ColorFromHex(0xFFFFFF), flashlightRadius, 15.0f),
    flashlightRadius,
    glm::cos(glm::radians(20.0f)),
    glm::cos(glm::radians(25.0f))
  ));

  m_dxRenderer->Init();
}

void Application::Cleanup() {

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
    //flashlight->direction = m_camera->GetFrontUnit();
    flashlight->axisFront = m_camera->GetFrontUnit();
    flashlight->axisRight = m_camera->GetRightUnit();
    flashlight->axisUp = m_camera->GetUpUnit();
  }

  // Update EV100
  const float evSpeed = 1.0f;

  if (m_input->IsKeyPressed(VK_OEM_PLUS)) {
    Flame::PostProcess::Get()->SetEvFactor(Flame::PostProcess::Get()->GetEvFactor() + evSpeed * deltaTime);
  }

  if (m_input->IsKeyPressed(VK_OEM_MINUS)) {
    Flame::PostProcess::Get()->SetEvFactor(Flame::PostProcess::Get()->GetEvFactor() - evSpeed * deltaTime);
  }

  const float kRotationSpeed = 5.0f;
  float rotationSpeed = kRotationSpeed;
  if (m_input->IsKeyPressed(VK_SHIFT)) {
    rotationSpeed *= 3;
  }

  if (m_input->IsKeyPressed('R')) {
    m_rotation += rotationSpeed * deltaTime;
    Flame::TransformSystem::Get()->At(m_planeTransformId)->transform.SetRotation(0, m_rotation, 0);
    Flame::TransformSystem::Get()->At(m_cubeTransformId)->transform.SetRotation(m_rotation, 12.0f, m_rotation);
  }
}

void Application::Render() const {
  m_dxRenderer->Render(m_time, m_deltaTime);

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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
