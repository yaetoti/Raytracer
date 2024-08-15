#include "MeshSystem.h"

#include "Flame/engine/TextureManager.h"
#include "Flame/graphics/DxContext.h"
#include "Flame/graphics/groups/TextureOnlyGroup.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "ModelManager.h"
#include <d3dcommon.h>
#include <wrl/client.h>

namespace Flame {
  MeshSystem::MeshSystem() {
  }

  void MeshSystem::Init() {
    // Opaque group
    {
      auto model1 = m_opaqueGroup.AddModel(ModelManager::Get()->GetModel("Assets/Models/Samurai/Samurai1.obj"));
      auto material1 = model1->AddMaterial({});
      material1->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(-2, -2, 0)) }) });
      material1->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(-2, 2, 0)) }) });
      material1->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(2, -2, 0)) }) });
      material1->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(2, 2, 0)) }) });
      auto material2 = model1->AddMaterial({});
      material2->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(0, 0, 2)) }) });
      material2->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(0, 0, -2)) }) });

      auto model2 = m_opaqueGroup.AddModel(ModelManager::Get()->GetModel("Assets/Cube.obj"));
      auto material3 = model2->AddMaterial({});
      material3->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(-2, -2, 2), glm::vec3(0.5f)) }) });
      material3->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(-2, 2, -2)) }) });
      material3->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(2, -2, -2), glm::vec3(0.1f)) }) });
      material3->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(2, 2, 2)) }) });
    }

    // Hologram group
    {
      auto model = m_hologramGroup.AddModel(ModelManager::Get()->GetModel("Assets/Models/Samurai/Samurai1.obj"));
      auto material1 = model->AddMaterial({});
      material1->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(0.0f), glm::vec3(0.5f)) }), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0) });
      material1->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(1.25f, 0.0f, 0.0f)) }), glm::vec3(0, 1, 0), glm::vec3(0, 1, 1) });
      
      auto model2 = m_hologramGroup.AddModel(ModelManager::Get()->GetModel("Assets/Cube.obj"));
      auto material2 = model2->AddMaterial({});
      material2->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(0.0f, -8.0f, 0.0f), glm::vec3(2.5f)) }), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1) });
      material2->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(-8.0f, 0.0f, 0.0f), glm::vec3(3.5f)) }), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) });
      material2->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(0.0f, 0.0f, -8.0f), glm::vec3(1.5f)) }), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) });
    }

    // TextureOnly group
    {
      auto model = m_textureOnlyGroup.AddModel(ModelManager::Get()->GetModel("Assets/Models/OtherCube/OtherCube.obj"));
      auto material0 = model->AddMaterial({ TextureManager::Get()->GetTexture(L"Assets/Models/OtherCube/OtherCube.dds")->GetResourceView() });
      material0->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(0.0f, 6.0f, 20.0f), glm::vec3(10.0f)) }) });
      auto material1 = model->AddMaterial({ TextureManager::Get()->GetTexture(L"Assets/Models/OtherCube/AnotherCube.dds")->GetResourceView() });
      material1->AddInstance({ TransformSystem::Get()->Insert({ Transform(glm::vec3(3.0f, 7.0f, 3.0f), glm::vec3(1.5f)) }) });
    }

    m_opaqueGroup.Init();
    m_hologramGroup.Init();
    m_textureOnlyGroup.Init();

    // Init skybox shaders
    m_skyVertexShader.Init(L"Assets/Shaders/sky.hlsl", nullptr, 0);
    m_skyPixelShader.Init(L"Assets/Shaders/sky.hlsl");
    m_textureView = TextureManager::Get()->GetTexture(kSkyboxPath)->GetResourceView();
  }

  void MeshSystem::Cleanup() {
    m_opaqueGroup.Cleanup();
    m_hologramGroup.Cleanup();
    m_textureOnlyGroup.Cleanup();
  }

  void MeshSystem::Update(float deltaTime) {

  }

  void MeshSystem::Render(float deltaTime) {
    m_opaqueGroup.Render();
    m_hologramGroup.Render();
    m_textureOnlyGroup.Render();

    RenderSkybox(deltaTime);
  }

  bool MeshSystem::Hit(const Ray& ray, HitRecord<HitResult>& record, float tMin, float tMax) const {
    HitRecord<OpaqueGroup::PerInstance*> opaqueResult;
    HitRecord<HologramGroup::PerInstance*> hologramResult;
    HitRecord<TextureOnlyGroup::PerInstance*> textureOnlyResult;
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
    if (m_textureOnlyGroup.HitInstance(ray, textureOnlyResult, tMin, tMax)) {
      wasHit |= true;
      tMax = textureOnlyResult.time;
      record = textureOnlyResult;
      record.data.groupType = GroupType::TEXTURE_ONLY_GROUP;
      record.data.perInstanceTextureOnly = textureOnlyResult.data;
    }

    return wasHit;
  }

  MeshSystem* MeshSystem::Get() {
    static MeshSystem instance;
    return &instance;
  }

  void MeshSystem::RenderSkybox(float deltaTime) {
    auto dc = DxContext::Get()->d3d11DeviceContext.Get();
    dc->VSSetShader(m_skyVertexShader.GetShader(), nullptr, 0);
    dc->PSSetShader(m_skyPixelShader.GetShader(), nullptr, 0);
    dc->IASetInputLayout(nullptr);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->PSSetShaderResources(0, 1, &m_textureView);
    dc->Draw(3, 0);
  }
}
