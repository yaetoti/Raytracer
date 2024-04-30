#pragma once
#include <glm/glm.hpp>

#include "Flame/math/IHitable.h"

namespace Flame {
  struct SceneObject final : IHitable {
    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      // TODO Transform ray into model space
      // TODO Hit mesh
      // TODO Transform results (normal and material?)
    }

    glm::mat4 GetModelMatrix() const {
      return m_modelMatrix;
    }

    glm::mat4 GetModelMatrixInv() const {
      return m_modelMatrixInv;
    }

    void SetModelMatrix(const glm::mat4& matrix) {
      m_modelMatrix = matrix;
      m_modelMatrixInv = glm::inverse(matrix);
    }

  private:
    glm::mat4 m_modelMatrix {};
    glm::mat4 m_modelMatrixInv {};
    uint32_t m_meshIndex;

  };
}
