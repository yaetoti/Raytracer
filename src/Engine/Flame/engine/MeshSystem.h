#pragma once
#include <d3d11.h>
#include <memory>
#include <glm/glm.hpp>
#include "Model.h"
#include "Flame/graphics/groups/HologramGroup.h"
#include "Flame/graphics/groups/OpaqueGroup.h"
#include "Flame/window/Window.h"

namespace Flame {
  enum class GroupType {
    OPAQUE_GROUP,
    HOLOGRAM_GROUP,
    COUNT
  };

  struct MeshSystem final {
    struct HitResult final {
      union {
        HologramGroup::PerInstance* perInstanceHologram;
        OpaqueGroup::PerInstance* perInstanceOpaque;
      };
      GroupType groupType;
    };

    MeshSystem();

    void Init();
    void Cleanup();
    void Update(float deltaTime);
    void Render(float deltaTime);

    bool Hit(const Ray& ray, HitRecord<HitResult>& record, float tMin, float tMax) const;

    static MeshSystem* Get();

  private:
    void RenderSkybox(float deltaTime);

  private:
    Window* m_window;

    OpaqueGroup m_opaqueGroup;
    HologramGroup m_hologramGroup;
    // Skybox
    VertexShader m_skyVertexShader;
    PixelShader m_skyPixelShader;
    ID3D11ShaderResourceView* m_textureView;

    static constexpr const wchar_t* kSkyboxPath = L"Assets/Textures/lake_beach.dds";
  };  
}
