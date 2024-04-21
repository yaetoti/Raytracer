#pragma once

#include "Layer.h"
#include "Flame/math/IHitable.h"

#include <memory>
#include <vector>
#include <Flame/math/Ray.h>
#include <Flame/render/RenderSurface.h>
#include <glm/glm.hpp>

namespace Flame {
  struct Scene3D : Layer {
  protected:
    std::vector<std::unique_ptr<IHitable>> m_hitables;

  private:
    void Render(RenderSurface& surface) override;
    glm::vec3 Color(const Ray& ray, int depth);
  };
}
