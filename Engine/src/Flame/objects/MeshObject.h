#pragma once

#include "Flame/math/IHitable.h"
#include "Flame/math/Mesh.h"

namespace Flame {
  struct MeshObject final : IHitable {
    MeshObject(const Mesh* mesh, int materialId);

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);
    void UpdateModelMatrix() const;

    glm::vec3 Position() const;
    glm::vec3 Rotation() const;
    glm::vec3 Scale() const;

  public:
    const Mesh* mesh;
    int materialId;

  private:
    // TODO move to SceneObject
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;

    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat4 m_modelMatrixInv;
    mutable bool m_modelMatrixDirty;
  };
}
