#pragma once

#include "Flame/components/Transform.h"
#include "Flame/math/IHitable.h"
#include "Flame/math/MeshOld.h"

namespace Flame {
  struct MeshObject final : IHitable {
    MeshObject(const MeshOld* mesh, int materialId);

    bool Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const override;

    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);
    void UpdateModelMatrix() const;

    glm::vec3 Position() const;
    glm::vec3 Rotation() const;
    glm::vec3 Scale() const;

  public:
    const MeshOld* mesh;
    int materialId;

  private:
    // TODO move to SceneObject
    Transform m_transform;

    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat4 m_modelMatrixInv;
    mutable bool m_modelMatrixDirty;
  };
}
