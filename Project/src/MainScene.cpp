#include "MainScene.h"

#include "Flame/math/MathUtils.h"
#include "Flame/math/Mesh.h"
#include "Flame/objects/MeshObject.h"
#include "Flame/math/Plane.h"
#include "Flame/objects/TriangleObject.h"
#include "Flame/objects/SphereObject.h"
#include "Flame/utils/ObjUtils.h"

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
     &m_materials[1]
   )
  );
  // Albedo sphere
  m_hitables.emplace_back(
   std::make_unique<Flame::SphereObject>(
     Flame::Sphere(glm::vec3(0, 1, -1), 0.3f),
     &m_materials[2]
   )
  );
  // The Big Danny
  m_hitables.emplace_back(
   std::make_unique<Flame::SphereObject>(
     Flame::Sphere(glm::vec3(0, -50.5, -1), 50.0f),
     &m_materials[3]
   )
  );
  // Plane
  m_hitables.emplace_back(
   std::make_unique<Flame::Plane>(
     glm::vec3(0, 0, -5),
     glm::normalize(glm::vec3(1, 0.2, 1)),
     &m_materials[4]
   )
  );
  //m_hitables.emplace_back(
  // std::make_unique<Flame::TriangleObject>(
  //   glm::vec3(0, 2, -4),
  //   glm::vec3(2, 2, -4),
  //   glm::vec3(1, 0, -4),
  //   glm::vec3(0, 0, 1),
  //   &m_materials[5]
  // )
  //);

  std::unique_ptr<Flame::IHitable>& cubeHitable = m_hitables.emplace_back(std::make_unique<Flame::MeshObject>(m_meshes[0].get(), &m_materials[6]));
  // TODO cringe
  Flame::MeshObject* cube = static_cast<Flame::MeshObject*>(cubeHitable.get());
  cube->SetPosition(glm::vec3(1.0f, 1.0f, 2.0f));
  cube->SetRotation(glm::radians(glm::vec3(37.0f, 45.0f, 12.0f)));

  std::unique_ptr<Flame::IHitable>& carHitable = m_hitables.emplace_back(std::make_unique<Flame::MeshObject>(m_meshes[1].get(), &m_materials[6]));
  // TODO cringe
  Flame::MeshObject* car = static_cast<Flame::MeshObject*>(carHitable.get());
  car->SetPosition(glm::vec3(5.0f, 0.0f, -3.0f));
  //car->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
  car->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
  // Car has correct rotation. The plane is under 45deg
  car->SetRotation(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));

  // TODO separate debug rendering? Add automatically
  Flame::Sphere lightSphere(
    glm::vec3(0.0f),
    0.1f
  );

  // Point lights
  {
    Flame::PointLight light;
    light.position = glm::vec3(5.0f, 6.0f, -3.0f);
    light.color = Flame::MathUtils::ColorFromHex(0xFA3120);
    light.intensity = 14.0f;
    light.constantFadeoff = 0.0f;
    light.linearFadeoff = 1.2f;
    light.quadraticFadeoff = 0.18f;
    m_pointLights.emplace_back(light);

    lightSphere.SetCenter(light.position);
    m_hitables.emplace_back(std::make_unique<Flame::SphereObject>(lightSphere, &m_materials[0]));
  }
  {
    Flame::PointLight light;
    light.position = glm::vec3(3.0f, 3.0f, 1.0f);
    light.color = Flame::MathUtils::ColorFromHex(0x2C1EF7);
    light.intensity = 12.0f;
    light.constantFadeoff = 0.0f;
    light.linearFadeoff = 0.7f;
    light.quadraticFadeoff = 0.18f;
    m_pointLights.emplace_back(light);

    lightSphere.SetCenter(light.position);
    m_hitables.emplace_back(std::make_unique<Flame::SphereObject>(lightSphere, &m_materials[0]));
  }

  // Spotlights
  {
    Flame::SpotLight light;
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light.position = glm::vec3(6.0f, 0.8f, -1.0f);
    light.direction = glm::vec3(-1.0f, 0.0f, 0.0f);
    light.intensity = 20.0f;
    light.cutoffCosineInner = glm::cos(glm::radians(5.0f));
    light.cutoffCosineOuter = glm::cos(glm::radians(12.0f));
    light.constantFadeoff = 0.0f;
    light.linearFadeoff = 0.2f;
    light.quadraticFadeoff = 0.04f;
    m_spotLights.emplace_back(light);

    lightSphere.SetCenter(light.position);
    m_hitables.emplace_back(std::make_unique<Flame::SphereObject>(lightSphere, &m_materials[0]));
  }

  // Direct lights
  {
    Flame::DirectLight light;
    light.color = glm::vec3(0.066666f, 0.070588f, 0.180392f);
    light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    light.intensity = 1.0f;
    m_directLights.emplace_back(light);
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
    Flame::Material m;
    m.albedo = glm::vec3(1.00f);
    m.emissionColor = glm::vec3(1.0f);
    m.emissionStrength = 1.0f;
    m.debugMaterial = true;
    m_materials.emplace_back(m);
  }
  {
    // Metal sphere
    Flame::Material m;
    m.albedo = glm::vec3(1.00f, 1.00f, 1.00f);
    m.diffuse = 1.0f;
    m.specular = 1.0f;
    m.specularExponent = 32.0f;
    m.roughness = 0.0f;
    m.metallic = 0.8f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    // Albedo sphere
    Flame::Material m;
    m.albedo = glm::vec3(0.28f, 1.0f, 0.5f);
    m.diffuse = 1.0f;
    m.specular = 1.0f;
    m.specularExponent = 32.0f;
    m.roughness = 1.0f;
    m.metallic = 0.0f;
    m.emissionColor = glm::vec3(0.28f, 1.0f, 0.5f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    // The big boi
    Flame::Material m;
    m.albedo = glm::vec3(1.0f, 0.28f, 0.5f);
    m.diffuse = 1.0f;
    m.specular = 1.0f;
    m.specularExponent = 32.0f;
    m.roughness = 1.0f;
    m.metallic = 0.1f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    // Plane
    Flame::Material m;
    m.albedo = glm::vec3(0.26f, 0.30f, 0.32f);
    m.diffuse = 1.0f;
    m.specular = 1.0f;
    m.specularExponent = 32.0f;
    m.roughness = 1.0f;
    m.metallic = 0.2f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    // TriangleObject
    Flame::Material m;
    m.albedo = glm::vec3(0.99f, 0.99f, 0.99f);
    m.diffuse = 1.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.0f;
    m.metallic = 1.0f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    // Cube
    Flame::Material m;
    m.albedo = glm::vec3(0.95f, 0.95f, 0.95f);
    m.diffuse = 1.0f;
    m.specular = 1.0f;
    m.specularExponent = 32.0f;
    m.roughness = 0.001f;
    m.metallic = 0.95f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    // Albedo MuscleCar
    Flame::Material m;
    m.albedo = glm::vec3(0.99f, 0.99f, 0.99f);
    m.diffuse = 1.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.1f;
    m.metallic = 0.8f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
}

void MainScene::InitializeMeshes() {
  {
    Flame::MeshData meshData;
    if (!Flame::ObjUtils::ParseObj(L"Assets/Cube.obj", meshData)) {
      std::wcout << L"Can't parse obj.\n";
      __debugbreak();
    }

    m_meshes.emplace_back(std::make_unique<Flame::Mesh>(meshData));
  }

  {
    Flame::MeshData meshData;
    // MuscleCar McLaren
    if (!Flame::ObjUtils::ParseObj(L"Assets/MuscleCar.obj", meshData)) {
      std::wcout << L"Can't parse obj.\n";
      __debugbreak();
    }

    m_meshes.emplace_back(std::make_unique<Flame::Mesh>(meshData));
  }
}
