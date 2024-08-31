#include "SceneOld.h"

#include "Flame/graphics/Material.h"

namespace Flame {
  const std::vector<std::unique_ptr<IHitable>>& SceneOld::GetHitables() const {
    return m_hitables;
  }

  std::vector<std::unique_ptr<IHitable>>& SceneOld::GetHitables() {
    return m_hitables;
  }

  const std::vector<std::unique_ptr<Material>>& SceneOld::GetMaterials() const {
    return m_materials;
  }

  const std::vector<std::unique_ptr<DirectLight>>& SceneOld::GetDirectLights() const {
    return m_directLights;
  }

  const std::vector<std::unique_ptr<PointLight>>& SceneOld::GetPointLights() const {
    return m_pointLights;
  }

  const std::vector<std::unique_ptr<SpotLight>>& SceneOld::GetSpotLights() const {
    return m_spotLights;
  }
}
