#include "MainScene.h"

#include "Flame/math/Cube.h"
#include "Flame/math/Plane.h"
#include "Flame/math/Triangle.h"
#include "Flame/render/AlbedoMaterial.h"
#include "Flame/render/LambertianMaterial.h"

MainScene::MainScene(Flame::Window& window)
: m_window(window)
, m_input(window.GetInputSystem()) {
}

void MainScene::Initialize() {
  // TODO Move fillers to Scene. Move filling to Application
  InitializeMaterials();

  // Objects
  m_hitables.emplace_back(
   std::make_unique<Flame::Sphere>(
     glm::vec3(0, 0, -2),
     0.5f,
     &m_materials[0]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Flame::Sphere>(
     glm::vec3(0, 1, -1),
     0.3f,
     &m_materials[1]
   )
  );
  m_hitables.emplace_back(
   std::make_unique<Flame::Sphere>(
     glm::vec3(0, -50.5, -1),
     50.0f,
     &m_materials[2]
   )
  );
  //m_hitables.emplace_back(
  // std::make_unique<Flame::Plane>(
  //   glm::vec3(0, 0, -5),
  //   glm::normalize(glm::vec3(1, 0.2, 1)),
  //   &m_materials[3]
  // )
  //);
  //m_hitables.emplace_back(
  // std::make_unique<Flame::Triangle>(
  //   glm::vec3(0, 2, -4),
  //   glm::vec3(2, 2, -4),
  //   glm::vec3(1, 0, -4),
  //   glm::vec3(0, 0, 1),
  //   &m_materials[4]
  // )
  //);
  //m_hitables.emplace_back(
  // std::make_unique<Flame::Cube>(
  //   &m_materials[5]
  // )
  //);

  // Point lights
  {
    Flame::PointLight light;
    light.position = glm::vec3(5.0f, 6.0f, -3.0f);
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light.intensity = 1000.0f;
    light.constantFadeoff = 1.0f;
    light.linearFadeoff = 1.0f;
    light.quadraticFadeoff = 1.0f;
    m_pointLights.emplace_back(light);
  }
}

void MainScene::Update(float deltaTime) {
}

void MainScene::Cleanup() {
}

void MainScene::InitializeMaterials() {
  // Materials
  {
    Flame::Material m;
    m.albedo = glm::vec3(0.90f, 0.90f, 0.90f);
    m.diffuse = 0.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.0f;
    m.metallic = 1.0f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    Flame::Material m;
    m.albedo = glm::vec3(0.28f, 1.0f, 0.5f);
    m.diffuse = 0.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.0f;
    m.metallic = 0.0f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    Flame::Material m;
    m.albedo = glm::vec3(1.0f, 0.28f, 0.5f);
    m.diffuse = 0.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.1f;
    m.metallic = 1.0f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    Flame::Material m;
    m.albedo = glm::vec3(0.26f, 0.30f, 0.32f);
    m.diffuse = 0.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.1f;
    m.metallic = 1.0f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    Flame::Material m;
    m.albedo = glm::vec3(0.99f, 0.99f, 0.99f);
    m.diffuse = 0.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.0f;
    m.metallic = 1.0f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
  {
    Flame::Material m;
    m.albedo = glm::vec3(0.99f, 0.99f, 0.99f);
    m.diffuse = 0.0f;
    m.specular = 0.0f;
    m.specularExponent = 0.0f;
    m.roughness = 0.0f;
    m.metallic = 1.0f;
    m.emissionColor = glm::vec3(0.0f);
    m.emissionStrength = 0.0f;
    m_materials.emplace_back(m);
  }
}
