#pragma once

#include "Layer.h"
#include "Flame/math/HitableList.h"

namespace Flame {
  struct Scene3D : Layer {

  protected:
    HitableList m_hitableList;

  private:
    void Render(RenderSurface& surface) override;
    glm::vec3 Color(const HitableList& hitaleList, const Ray& ray, int depth);
  };
}
