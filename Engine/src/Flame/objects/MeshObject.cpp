#include "MeshObject.h"

namespace Flame {
  MeshObject::MeshObject(const Mesh* mesh, int materialId)
  : mesh(mesh)
  , materialId(materialId)
  , m_position(0.0f)
  , m_rotation(0.0f)
  , m_scale(1.0f)
  , m_modelMatrix()
  , m_modelMatrixInv()
  , m_modelMatrixDirty(true) {
  }

  bool MeshObject::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
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
    m_position = position;
    m_modelMatrixDirty = true;
  }

  void MeshObject::SetRotation(glm::vec3 rotation) {
    m_rotation = rotation;
    m_modelMatrixDirty = true;
  }

  void MeshObject::SetScale(glm::vec3 scale) {
    m_scale = scale;
    m_modelMatrixDirty = true;
  }

  void MeshObject::UpdateModelMatrix() const {
    m_modelMatrix = glm::translate(m_position)
      * glm::eulerAngleZXY(m_rotation.z, m_rotation.x, m_rotation.y)
      * glm::scale(m_scale);
    m_modelMatrixInv = glm::inverse(m_modelMatrix);
    m_modelMatrixDirty = false;
  }

  glm::vec3 MeshObject::Position() const {
    return m_position;
  }

  glm::vec3 MeshObject::Rotation() const {
    return m_rotation;
  }

  glm::vec3 MeshObject::Scale() const {
    return m_scale;
  }
}
