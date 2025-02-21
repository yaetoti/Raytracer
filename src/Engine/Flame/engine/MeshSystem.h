#pragma once
#include <d3d11.h>
#include <memory>
#include <glm/glm.hpp>

#include "IShadowMapProvider.h"
#include "Flame/graphics/groups/TextureOnlyGroup.h"
#include "Flame/graphics/groups/EmissionOnlyGroup.h"
#include "Model.h"
#include "Flame/graphics/groups/HologramGroup.h"
#include "Flame/graphics/groups/OpaqueGroup.h"
#include "Flame/window/Window.h"

namespace Flame {
  enum class GroupType {
    OPAQUE_GROUP,
    HOLOGRAM_GROUP,
    TEXTURE_ONLY_GROUP,
    EMISSION_ONLY_GROUP,
    COUNT
  };

  struct MeshSystem final {
    struct HitResult final {
      union {
        OpaqueGroup::PerInstance* perInstanceOpaque;
        HologramGroup::PerInstance* perInstanceHologram;
        TextureOnlyGroup::PerInstance* perInstanceTextureOnly;
        EmissionOnlyGroup::PerInstance* perInstanceEmissionOnly;
      };
      GroupType groupType;
    };

    MeshSystem();

    void Init();
    void Cleanup();
    void Update(float deltaTime);
    void Render(float deltaTime);
    void RenderDepth2D();
    void RenderDepthCubemaps(std::span<glm::vec3> positions);

    OpaqueGroup* GetOpaqueGroup();
    HologramGroup* GetHologramGroup();
    TextureOnlyGroup* GetTextureOnlyGroup();
    EmissionOnlyGroup* GetEmissionOnlyGroup();

    bool Hit(const Ray& ray, HitRecord<HitResult>& record, float tMin, float tMax) const;

    void SetShadowMapProvider(const std::shared_ptr<IShadowMapProvider>& provider);

    static MeshSystem* Get();

  private:
    Window* m_window;

    OpaqueGroup m_opaqueGroup;
    HologramGroup m_hologramGroup;
    TextureOnlyGroup m_textureOnlyGroup;
    EmissionOnlyGroup m_emissionOnlyGroup;

    std::shared_ptr<IShadowMapProvider> m_shadowMapProvider;
  };
}
