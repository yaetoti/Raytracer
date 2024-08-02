#include "MainScene.h"

MainScene::MainScene(Flame::Window& window)
: m_window(window)
, m_input(window.GetInputSystem()) {
}

void MainScene::Initialize() {
  // TODO Move fillers to Scene. Move filling to Application
  InitializeMaterials();
  InitializeMeshes();

  // Objects
  // Metallic sphere
  m_hitables.emplace_back(
   std::make_unique<Flame::SphereObject>(
     Flame::Sphere(glm::vec3(0, 0, -2), 0.5f),
     1
   )
  );
  // Albedo sphere
  m_hitables.emplace_back(
   std::make_unique<Flame::SphereObject>(
     Flame::Sphere(glm::vec3(0, 1, -1), 0.3f),
     2
   )
  );
  // The Big Danny
  m_hitables.emplace_back(
   std::make_unique<Flame::SphereObject>(
     Flame::Sphere(glm::vec3(0, -50.5, -1), 50.0f),
     3
   )
  );
  // Plane
  m_hitables.emplace_back(
   std::make_unique<Flame::PlaneObject>(
     Flame::Plane(glm::vec3(0, 0, -5), glm::normalize(glm::vec3(1, 0.2, 1))),
     4
   )
  );
  // TriangleOld
  m_hitables.emplace_back(
   std::make_unique<Flame::TriangleObject>(
     glm::vec3(0, 2, -4),
     glm::vec3(2, 2, -4),
     glm::vec3(1, 0, -4),
     glm::vec3(0, 0, 1),
     5
   )
  );
  // Cube
  {
    auto object = std::make_unique<Flame::MeshObject>(m_meshes[0].get(), 6);
    object->SetPosition(glm::vec3(1.0f, 1.0f, 2.0f));
    object->SetRotation(glm::radians(glm::vec3(37.0f, 45.0f, 12.0f)));
    m_hitables.emplace_back(std::move(object));
  }
  // Car
  {
    auto object = std::make_unique<Flame::MeshObject>(m_meshes[1].get(), 6);
    object->SetPosition(glm::vec3(5.0f, 0.0f, -3.0f));
    //object->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
    object->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
    // Car has correct rotation. The plane is at 45deg
    object->SetRotation(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
    m_hitables.emplace_back(std::move(object));
  }

  // TODO separate debug rendering? Add automatically
  Flame::Sphere lightSphere(
    glm::vec3(0.0f),
    0.1f
  );

  // Point lights
  {
    auto light = std::make_unique<Flame::PointLight>();
    light->position = glm::vec3(5.0f, 6.0f, -3.0f);
    light->color = Flame::MathUtils::ColorFromHex(0xFA3120);
    light->intensity = 14.0f;
    light->constantFadeoff = 0.0f;
    light->linearFadeoff = 1.2f;
    light->quadraticFadeoff = 0.18f;
    lightSphere.center = light->position;

    m_pointLights.emplace_back(std::move(light));
    m_hitables.emplace_back(std::make_unique<Flame::SphereObject>(lightSphere, 0));
  }
  {
    auto light = std::make_unique<Flame::PointLight>();
    light->position = glm::vec3(3.0f, 3.0f, 1.0f);
    light->color = Flame::MathUtils::ColorFromHex(0x2C1EF7);
    light->intensity = 12.0f;
    light->constantFadeoff = 0.0f;
    light->linearFadeoff = 0.7f;
    light->quadraticFadeoff = 0.18f;
    lightSphere.center = light->position;

    m_pointLights.emplace_back(std::move(light));
    m_hitables.emplace_back(std::make_unique<Flame::SphereObject>(lightSphere, 0));
  }

  // Spotlights
  {
    auto light = std::make_unique<Flame::SpotLight>();
    light->color = glm::vec3(1.0f, 1.0f, 1.0f);
    light->position = glm::vec3(6.0f, 0.8f, -1.0f);
    light->direction = glm::vec3(-1.0f, 0.0f, 0.0f);
    light->intensity = 20.0f;
    light->cutoffCosineInner = glm::cos(glm::radians(5.0f));
    light->cutoffCosineOuter = glm::cos(glm::radians(12.0f));
    light->constantFadeoff = 0.0f;
    light->linearFadeoff = 0.2f;
    light->quadraticFadeoff = 0.04f;
    lightSphere.center = light->position;

    m_spotLights.emplace_back(std::move(light));
    m_hitables.emplace_back(std::make_unique<Flame::SphereObject>(lightSphere, 0));
  }

  // Direct lights
  {
    auto light = std::make_unique<Flame::DirectLight>();
    light->color = glm::vec3(0.066666f, 0.070588f, 0.180392f);
    light->direction = glm::vec3(0.0f, -1.0f, 0.0f);
    light->intensity = 1.0f;
    m_directLights.emplace_back(std::move(light));
  }
}

void MainScene::Update(float deltaTime) {
}

void MainScene::Cleanup() {
}

void MainScene::InitializeMaterials() {
  // Materials
  {
    // Light Visualizer
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(1.00f);
    m->emissionColor = glm::vec3(1.0f);
    m->emissionStrength = 1.0f;
    m_materials.emplace_back(std::move(m));
  }
  {
    // Metal sphere
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(1.00f, 1.00f, 1.00f);
    m->diffuse = 1.0f;
    m->specular = 1.0f;
    m->specularExponent = 32.0f;
    m->roughness = 0.0f;
    m->metallic = 0.8f;
    m->emissionColor = glm::vec3(0.0f);
    m->emissionStrength = 0.0f;
    m_materials.emplace_back(std::move(m));
  }
  {
    // Albedo sphere
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(0.28f, 1.0f, 0.5f);
    m->diffuse = 1.0f;
    m->specular = 1.0f;
    m->specularExponent = 32.0f;
    m->roughness = 1.0f;
    m->metallic = 0.0f;
    m->emissionColor = glm::vec3(0.28f, 1.0f, 0.5f);
    m->emissionStrength = 0.0f;
    m_materials.emplace_back(std::move(m));
  }
  {
    // The big boi
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(1.0f, 0.28f, 0.5f);
    m->diffuse = 1.0f;
    m->specular = 1.0f;
    m->specularExponent = 32.0f;
    m->roughness = 1.0f;
    m->metallic = 0.1f;
    m->emissionColor = glm::vec3(0.0f);
    m->emissionStrength = 0.0f;
    m_materials.emplace_back(std::move(m));
  }
  {
    // Plane
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(0.26f, 0.30f, 0.32f);
    m->diffuse = 1.0f;
    m->specular = 1.0f;
    m->specularExponent = 32.0f;
    m->roughness = 1.0f;
    m->metallic = 0.2f;
    m->emissionColor = glm::vec3(0.0f);
    m->emissionStrength = 0.0f;
    m_materials.emplace_back(std::move(m));
  }
  {
    // TriangleObject
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(0.99f, 0.99f, 0.99f);
    m->diffuse = 1.0f;
    m->specular = 0.0f;
    m->specularExponent = 0.0f;
    m->roughness = 0.0f;
    m->metallic = 1.0f;
    m->emissionColor = glm::vec3(0.0f);
    m->emissionStrength = 0.0f;
    m_materials.emplace_back(std::move(m));
  }
  {
    // 6 Cube
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(0.95f, 0.95f, 0.95f);
    m->diffuse = 1.0f;
    m->specular = 1.0f;
    m->specularExponent = 32.0f;
    m->roughness = 0.001f;
    m->metallic = 0.95f;
    m->emissionColor = glm::vec3(0.0f);
    m->emissionStrength = 0.0f;
    m_materials.emplace_back(std::move(m));
  }
  {
    // 7 Albedo MuscleCar
    auto m = std::make_unique<Flame::Material>();
    m->albedo = glm::vec3(0.99f, 0.99f, 0.99f);
    m->diffuse = 1.0f;
    m->specular = 0.0f;
    m->specularExponent = 0.0f;
    m->roughness = 0.1f;
    m->metallic = 0.8f;
    m->emissionColor = glm::vec3(0.0f);
    m->emissionStrength = 0.0f;
    m_materials.emplace_back(std::move(m));
  }
}

void MainScene::InitializeMeshes() {
  {
    Flame::MeshData meshData;
    if (!Flame::ObjUtils::ParseObj(L"Assets/Cube.obj", meshData)) {
      std::wcout << L"Can't parse obj.\n";
      __debugbreak();
    }

    m_meshes.emplace_back(std::make_unique<Flame::MeshOld>(meshData));
  }

  {
    Flame::MeshData meshData;
    // MuscleCar McLaren
    if (!Flame::ObjUtils::ParseObj(L"Assets/MuscleCar.obj", meshData)) {
      std::wcout << L"Can't parse obj.\n";
      __debugbreak();
    }

    m_meshes.emplace_back(std::make_unique<Flame::MeshOld>(meshData));
  }
}
