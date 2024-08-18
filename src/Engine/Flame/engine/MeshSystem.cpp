#include "MeshSystem.h"

#include "Flame/engine/lights/PointLight.h"
#include "Flame/graphics/groups/EmissionOnlyGroup.h"
#include "Flame/graphics/groups/ShaderGroup.h"
#include "Flame/math/MathUtils.h"
#include "LightSystem.h"
#include "Flame/engine/TextureManager.h"
#include "Flame/graphics/DxContext.h"
#include "Flame/graphics/groups/TextureOnlyGroup.h"
#include "Flame/graphics/shaders/PixelShader.h"
#include "Flame/graphics/shaders/VertexShader.h"
#include "ModelManager.h"
#include "glm/fwd.hpp"
#include "lights/DirectLight.h"
#include <d3dcommon.h>
#include <wrl/client.h>

namespace Flame {
  MeshSystem::MeshSystem() {
  }

  void MeshSystem::Init() {
    m_opaqueGroup.Init();
    m_hologramGroup.Init();
    m_emissionOnlyGroup.Init();
    m_textureOnlyGroup.Init();

    // Init skybox shaders
    m_skyVertexShader.Init(L"Assets/Shaders/sky.hlsl", nullptr, 0);
    m_skyPixelShader.Init(L"Assets/Shaders/sky.hlsl");
    m_textureView = TextureManager::Get()->GetTexture(kSkyboxPath)->GetResourceView();

    // Init light TODO: move
    LightSystem::Get()->AddDirectLight(std::make_shared<DirectLight>(
      MathUtils::ColorFromHex(0x000000),
      glm::vec3(0, -1, 0),
      1.0f
    ));

    LightSystem::Get()->AddSpotLight(std::make_shared<SpotLight>(
      glm::vec3(0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),
      MathUtils::ColorFromHex(0xFFFFFF),
      5.0f,
      0.707f,
      0.642f,
      0.0f, 1.2f, 0.18f
    ));
  }

  void MeshSystem::Cleanup() {
    m_opaqueGroup.Cleanup();
    m_hologramGroup.Cleanup();
    m_emissionOnlyGroup.Cleanup();
    m_textureOnlyGroup.Cleanup();
  }

  void MeshSystem::Update(float deltaTime) {

  }

  void MeshSystem::Render(float deltaTime) {
    m_opaqueGroup.Render();
    m_hologramGroup.Render();
    m_emissionOnlyGroup.Render();
    m_textureOnlyGroup.Render();

    RenderSkybox(deltaTime);
  }

  OpaqueGroup* MeshSystem::GetOpaqueGroup() {
    return &m_opaqueGroup;
  }

  HologramGroup* MeshSystem::GetHologramGroup() {
    return &m_hologramGroup;
  }

  TextureOnlyGroup* MeshSystem::GetTextureOnlyGroup() {
    return &m_textureOnlyGroup;
  }

  EmissionOnlyGroup* MeshSystem::GetEmissionOnlyGroup() {
    return &m_emissionOnlyGroup;
  }

  bool MeshSystem::Hit(const Ray& ray, HitRecord<HitResult>& record, float tMin, float tMax) const {
    HitRecord<OpaqueGroup::PerInstance*> opaqueResult;
    HitRecord<HologramGroup::PerInstance*> hologramResult;
    HitRecord<TextureOnlyGroup::PerInstance*> textureOnlyResult;
    HitRecord<EmissionOnlyGroup::PerInstance*> emissionOnlyResult;
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
    if (m_emissionOnlyGroup.HitInstance(ray, emissionOnlyResult, tMin, tMax)) {
      wasHit |= true;
      tMax = emissionOnlyResult.time;
      record = emissionOnlyResult;
      record.data.groupType = GroupType::EMISSION_ONLY_GROUP;
      record.data.perInstanceEmissionOnly = emissionOnlyResult.data;
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
