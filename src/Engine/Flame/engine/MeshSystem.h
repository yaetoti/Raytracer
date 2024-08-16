#pragma once
#include <d3d11.h>
#include <memory>
#include <glm/glm.hpp>
#include "Flame/graphics/groups/TextureOnlyGroup.h"
#include "Model.h"
#include "Flame/graphics/groups/HologramGroup.h"
#include "Flame/graphics/groups/OpaqueGroup.h"
#include "Flame/window/Window.h"

namespace Flame {
  enum class GroupType {
    OPAQUE_GROUP,
    HOLOGRAM_GROUP,
    TEXTURE_ONLY_GROUP,
    COUNT
  };

  struct MeshSystem final {
    struct HitResult final {
      union {
        OpaqueGroup::PerInstance* perInstanceOpaque;
        HologramGroup::PerInstance* perInstanceHologram;
        TextureOnlyGroup::PerInstance* perInstanceTextureOnly;
      };
      GroupType groupType;
    };

    MeshSystem();

    void Init();
    void Cleanup();
    void Update(float deltaTime);
    void Render(float deltaTime);

    OpaqueGroup* GetOpaqueGroup();
    HologramGroup* GetHologramGroup();
    TextureOnlyGroup* GetTextureOnlyGroup();

    bool Hit(const Ray& ray, HitRecord<HitResult>& record, float tMin, float tMax) const;

    static MeshSystem* Get();

  private:
    void RenderSkybox(float deltaTime);

  private:
    Window* m_window;

    OpaqueGroup m_opaqueGroup;
    HologramGroup m_hologramGroup;
    TextureOnlyGroup m_textureOnlyGroup;
    // Skybox
    VertexShader m_skyVertexShader;
    PixelShader m_skyPixelShader;
    ID3D11ShaderResourceView* m_textureView;

    static constexpr const wchar_t* kSkyboxPath = L"Assets/Textures/lake_beach.dds";
  };  
}
