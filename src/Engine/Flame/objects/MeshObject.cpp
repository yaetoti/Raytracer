#include "MeshObject.h"

#include "Flame/math/MathUtils.h"

namespace Flame {
  MeshObject::MeshObject(const MeshOld* mesh, int materialId)
  : mesh(mesh)
  , materialId(materialId)
  , m_modelMatrix()
  , m_modelMatrixInv()
  , m_modelMatrixDirty(true) {
  }

  bool MeshObject::Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const {
    if (m_modelMatrixDirty) {
      UpdateModelMatrix();
    }

    glm::vec4 origin = m_modelMatrixInv * glm::vec4(r.origin, 1.0f);
    glm::vec4 direction = m_modelMatrixInv * glm::vec4(r.direction, 0.0f);
    Ray ray(origin / origin.w, direction);

    if (mesh->Hit(ray, record, tMin, tMax)) {
      record.materialId = materialId;
      record.hitable = const_cast<MeshObject*>(this);
      // TODO move transformation into SceneObject
      glm::vec4 normal = glm::normalize(m_modelMatrix * glm::vec4(record.normal, 0.0f));
      glm::vec4 point = m_modelMatrix * glm::vec4(record.point, 1.0f);
      record.normal = normal;
      record.point = point / point.w;
      return true;
    }

    return false;
  }

  void MeshObject::SetPosition(glm::vec3 position) {
    m_transform.SetPosition(position);
    m_modelMatrixDirty = true;
  }

  void MeshObject::SetRotation(glm::vec3 rotation) {
    m_transform.SetRotation(rotation);
    m_modelMatrixDirty = true;
  }

  void MeshObject::SetScale(glm::vec3 scale) {
    m_transform.SetScale(scale);
    m_modelMatrixDirty = true;
  }

  void MeshObject::UpdateModelMatrix() const {
    m_modelMatrix = MathUtils::Translate(m_transform.GetPosition())
      * m_transform.GetRotationMat()
      * MathUtils::Scale(m_transform.GetScale());
    m_modelMatrixInv = glm::inverse(m_modelMatrix);
    m_modelMatrixDirty = false;
  }

  glm::vec3 MeshObject::Position() const {
    return m_transform.GetPosition();
  }

  glm::vec3 MeshObject::Rotation() const {
    return m_transform.GetRotationEuler();
  }

  glm::vec3 MeshObject::Scale() const {
    return m_transform.GetScale();
  }
}
