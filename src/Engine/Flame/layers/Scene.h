#pragma once

#include "Flame/math/IHitable.h"

#include <memory>
#include <vector>

#include "Flame/graphics/Material.h"

namespace Flame {
  struct Scene {
    virtual ~Scene() = default;

    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Cleanup() {}

    const std::vector<std::unique_ptr<IHitable>>& GetHitables() const;
    std::vector<std::unique_ptr<IHitable>>& GetHitables(); // TODO remove. Replace MathUtils::HitClosest with scene method
    const std::vector<std::unique_ptr<Material>>& GetMaterials() const;
    const std::vector<std::unique_ptr<DirectLight>>& GetDirectLights() const;
    const std::vector<std::unique_ptr<PointLight>>& GetPointLights() const;
    const std::vector<std::unique_ptr<SpotLight>>& GetSpotLights() const;

  protected:
    std::vector<std::unique_ptr<IHitable>> m_hitables;
    std::vector<std::unique_ptr<Material>> m_materials;

    std::vector<std::unique_ptr<DirectLight>> m_directLights;
    std::vector<std::unique_ptr<PointLight>> m_pointLights;
    std::vector<std::unique_ptr<SpotLight>> m_spotLights;
  };
}
