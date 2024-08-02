#include "Scene.h"

#include "Flame/graphics/Material.h"

namespace Flame {
  const std::vector<std::unique_ptr<IHitable>>& Scene::GetHitables() const {
    return m_hitables;
  }

  std::vector<std::unique_ptr<IHitable>>& Scene::GetHitables() {
    return m_hitables;
  }

  const std::vector<std::unique_ptr<Material>>& Scene::GetMaterials() const {
    return m_materials;
  }

  const std::vector<std::unique_ptr<DirectLight>>& Scene::GetDirectLights() const {
    return m_directLights;
  }

  const std::vector<std::unique_ptr<PointLight>>& Scene::GetPointLights() const {
    return m_pointLights;
  }

  const std::vector<std::unique_ptr<SpotLight>>& Scene::GetSpotLights() const {
    return m_spotLights;
  }
}
