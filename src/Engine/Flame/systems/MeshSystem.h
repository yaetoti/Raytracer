#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "ConstantBuffer.h"
#include "HologramGroup.h"
#include "Model.h"
#include "OpaqueGroup.h"
#include "PixelShader.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Flame/window/Window.h"

namespace Flame {
  struct MeshSystem final {
    MeshSystem();

    void Init();
    void Cleanup();
    void Update(float deltaTime);
    void Render(float deltaTime);

    bool Hit(const Ray& ray, HitRecord& record, float tMin, float tMax) const;

    static MeshSystem* Get();

  private:
    Window* m_window;

    OpaqueGroup m_opaqueGroup;
    HologramGroup m_hologramGroup;
  };  
}
