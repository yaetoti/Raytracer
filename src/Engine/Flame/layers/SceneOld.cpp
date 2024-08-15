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
}
