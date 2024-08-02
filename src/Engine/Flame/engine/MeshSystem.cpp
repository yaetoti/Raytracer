#include "MeshSystem.h"

#include "ModelManager.h"

namespace Flame {
  MeshSystem::MeshSystem() {
  }

  void MeshSystem::Init() {
    // Opaque group
    {
      auto& model1 = m_opaqueGroup.AddModel("Samurai", ModelManager::Get()->GetModel("Assets/Models/Samurai/Samurai1.obj"));
      auto& material1 = model1->AddMaterial("Default", {});
      material1->AddInstance("Wu", { glm::translate(glm::vec3(-2, -2, 0)) });
      material1->AddInstance("Chu", { glm::translate(glm::vec3(-2, 2, 0)) });
      material1->AddInstance("Yu", { glm::translate(glm::vec3(2, -2, 0)) });
      material1->AddInstance("Lu", { glm::translate(glm::vec3(2, 2, 0)) });
      auto& material2 = model1->AddMaterial("Custom", {});
      material2->AddInstance("Yan", { glm::translate(glm::vec3(0, 0, 2)) });
      material2->AddInstance("Chen", { glm::translate(glm::vec3(0, 0, -2)) });

      auto& model2 = m_opaqueGroup.AddModel("Cube", ModelManager::Get()->GetModel("Assets/Cube.obj"));
      auto& material3 = model2->AddMaterial("Simple", {});
      material3->AddInstance("Cube", { glm::translate(glm::vec3(-2, -2, 2)) * glm::scale(glm::vec3(0.5f)) });
      material3->AddInstance("Square", { glm::translate(glm::vec3(-2, 2, -2)) });
      material3->AddInstance("Box", { glm::translate(glm::vec3(2, -2, -2)) * glm::scale(glm::vec3(0.1f)) });
      material3->AddInstance("Parallelogram", { glm::translate(glm::vec3(2, 2, 2)) });
    }

    // Yu, Chu, Woo, Ling, Chen, Ming, Tao, Wei, Yun, Jian, Li, Zhen, Hao, Feng, Ren, Xing, Guang, Zhi, Ping, Hong, Shen, Lian, Bo, An, Ning, Wen, Qiang, Rui, Lan, Jing, Bao, Hui, Kang, Shan, De, Shun, Le, Si, Yong, Xiu, Fu, Tie, Ge, Ping, Wei, Mao, Yuan, Gao, Guo, Shuo and others
    // Run in release mode. It doesn't have duplicate check for names
    //uint32_t index = 0;
    //for (int x = -10; x < 10; ++x) {
    //  for (int y = -10; y < 10; ++y) {
    //    for (int z = -10; z < 10; ++z) {
    //      material3->AddInstance(
    //        "",
    //        { glm::translate(glm::vec3(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 2.0f, static_cast<float>(z) * 2.0f)) * glm::scale(glm::vec3(1.0f)) }
    //      );
    //      ++index;
    //    }
    //  }
    //}

    // Hologram group
    {
      auto& model = m_hologramGroup.AddModel("Samurai", ModelManager::Get()->GetModel("Assets/Models/Samurai/Samurai1.obj"));
      auto& material1 = model->AddMaterial("Default", {});
      material1->AddInstance("Po", { glm::translate(glm::vec3(0, 0, 0)), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0) });
      material1->AddInstance("Lmao", { glm::translate(glm::vec3(1.25, 0, 0)), glm::vec3(0, 1, 0), glm::vec3(0, 1, 1) });
    }

    m_opaqueGroup.Init();
    m_hologramGroup.Init();
  }

  void MeshSystem::Cleanup() {
    m_opaqueGroup.Cleanup();
    m_hologramGroup.Cleanup();
  }

  void MeshSystem::Update(float deltaTime) {

  }

  void MeshSystem::Render(float deltaTime) {
    m_opaqueGroup.Render();
    m_hologramGroup.Render();
  }

  bool MeshSystem::Hit(const Ray& ray, HitRecord& record, float tMin, float tMax) const {
    bool wasHit = m_opaqueGroup.HitInstance(ray, record, tMin, tMax);
    if (wasHit) {
      tMax = record.time;
    }
    wasHit |= m_hologramGroup.HitInstance(ray, record, tMin, tMax);

    return wasHit;
  }

  MeshSystem* MeshSystem::Get() {
    static MeshSystem instance;
    return &instance;
  }
}
