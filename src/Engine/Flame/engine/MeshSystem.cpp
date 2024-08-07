#include "MeshSystem.h"

#include "ModelManager.h"

namespace Flame {
  MeshSystem::MeshSystem() {
  }

  void MeshSystem::Init() {
    // Opaque group
    {
      auto model1 = m_opaqueGroup.AddModel(ModelManager::Get()->GetModel("Assets/Models/Samurai/Samurai1.obj"));
      auto material1 = model1->AddMaterial({});
      material1->AddInstance({ Transform(glm::vec3(-2, -2, 0)) });
      material1->AddInstance({ Transform(glm::vec3(-2, 2, 0)) });
      material1->AddInstance({ Transform(glm::vec3(2, -2, 0)) });
      material1->AddInstance({ Transform(glm::vec3(2, 2, 0)) });
      auto material2 = model1->AddMaterial({});
      material2->AddInstance({ Transform(glm::vec3(0, 0, 2)) });
      material2->AddInstance({ Transform(glm::vec3(0, 0, -2)) });

      auto model2 = m_opaqueGroup.AddModel(ModelManager::Get()->GetModel("Assets/Cube.obj"));
      auto material3 = model2->AddMaterial({});
      material3->AddInstance({ Transform(glm::vec3(-2, -2, 2), glm::vec3(0.5f)) });
      material3->AddInstance({ Transform(glm::vec3(-2, 2, -2)) });
      material3->AddInstance({ Transform(glm::vec3(2, -2, -2), glm::vec3(0.1f)) });
      material3->AddInstance({ Transform(glm::vec3(2, 2, 2)) });
    }

    // Yu, Chu, Woo, Ling, Chen, Ming, Tao, Wei, Yun, Jian, Li, Zhen, Hao, Feng, Ren, Xing, Guang, Zhi, Ping, Hong, Shen, Lian, Bo, An, Ning, Wen, Qiang, Rui, Lan, Jing, Bao, Hui, Kang, Shan, De, Shun, Le, Si, Yong, Xiu, Fu, Tie, Ge, Ping, Wei, Mao, Yuan, Gao, Guo, Shuo and others
    //{
    //  auto model1 = m_opaqueGroup.AddModel(ModelManager::Get()->GetModel("Assets/Models/Samurai/Samurai1.obj"));
    //  auto material1 = model1->AddMaterial({});
    //  uint32_t index = 0;
    //  for (int x = -10; x < 10; ++x) {
    //    for (int y = -10; y < 10; ++y) {
    //      for (int z = -10; z < 10; ++z) {
    //        material1->AddInstance({ Transform(glm::vec3(x * 2.0f, y * 2.0f, z * 2.0f), glm::vec3(glm::vec3(0.2f))) });
    //        ++index;
    //      }
    //    }
    //  }
    //  std::cout << index << '\n';
    //}

    // Hologram group
    {
      auto model = m_hologramGroup.AddModel(ModelManager::Get()->GetModel("Assets/Models/Samurai/Samurai1.obj"));
      auto material1 = model->AddMaterial({});
      material1->AddInstance({ Transform(glm::vec3(0.0f), glm::vec3(0.5f)), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0) });
      material1->AddInstance({ Transform(glm::vec3(1.25f, 0.0f, 0.0f)), glm::vec3(0, 1, 0), glm::vec3(0, 1, 1) });
      
      auto model2 = m_hologramGroup.AddModel(ModelManager::Get()->GetModel("Assets/Cube.obj"));
      auto material2 = model2->AddMaterial({});
      material2->AddInstance({ Transform(glm::vec3(0.0f, -8.0f, 0.0f), glm::vec3(2.5f)), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1) });
      material2->AddInstance({ Transform(glm::vec3(-8.0f, 0.0f, 0.0f), glm::vec3(3.5f)), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) });
      material2->AddInstance({ Transform(glm::vec3(0.0f, 0.0f, -8.0f), glm::vec3(1.5f)), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) });

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
    // TODO Render Skybox:
    // Bind empty layout
    // Bind VS and PS shaders
    // Draw 3 vertices

    // Pass camera frustum vectors to a constant buffer
    // In VS assign VSOutput direction that will be interpolated
  }

  bool MeshSystem::Hit(const Ray& ray, HitRecord<HitResult>& record, float tMin, float tMax) const {
    HitRecord<OpaqueGroup::PerInstance*> opaqueResult;
    HitRecord<HologramGroup::PerInstance*> hologramResult;
    bool wasHit = false;

    if (m_opaqueGroup.HitInstance(ray, opaqueResult, tMin, tMax)) {
      wasHit |= true;
      tMax = opaqueResult.time;
      record = opaqueResult;
      record.data.groupType = GroupType::OPAQUE_GROUP;
      record.data.perInstanceOpaque = opaqueResult.data;
    }
    if (m_hologramGroup.HitInstance(ray, hologramResult, tMin, tMax)) {
      wasHit |= true;
      tMax = hologramResult.time;
      record = hologramResult;
      record.data.groupType = GroupType::HOLOGRAM_GROUP;
      record.data.perInstanceHologram = hologramResult.data;
    }

    return wasHit;
  }

  MeshSystem* MeshSystem::Get() {
    static MeshSystem instance;
    return &instance;
  }
}
