#pragma once
#include "IHitable.h"
#include "Mesh.h"

namespace Flame {
  struct MeshObject final : IHitable {
    MeshObject(const Mesh* mesh, const Material* material)
    : mesh(mesh)
    , material(material)
    , m_position(0.0f)
    , m_rotation(0.0f)
    , m_scale(1.0f)
    , m_modelMatrix()
    , m_modelMatrixInv()
    , m_modelMatrixDirty(true) {
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      if (m_modelMatrixDirty) {
        UpdateModelMatrix();
      }

      glm::vec4 origin = m_modelMatrixInv * glm::vec4(r.origin, 1.0f);
      glm::vec4 direction = m_modelMatrixInv * glm::vec4(r.direction, 0.0f);
      Ray ray(origin / origin.w, direction);

      if (mesh->Hit(ray, record, tMin, tMax)) {
        record.material = material;
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

    void SetPosition(glm::vec3 position) {
      m_position = position;
      m_modelMatrixDirty = true;
    }

    void SetRotation(glm::vec3 rotation) {
      m_rotation = rotation;
      m_modelMatrixDirty = true;
    }

    void SetScale(glm::vec3 scale) {
      m_scale = scale;
      m_modelMatrixDirty = true;
    }

    void UpdateModelMatrix() const {
      m_modelMatrix = glm::translate(m_position)
        * glm::eulerAngleZXY(m_rotation.z, m_rotation.x, m_rotation.y)
        * glm::scale(m_scale);
      m_modelMatrixInv = glm::inverse(m_modelMatrix);
      m_modelMatrixDirty = false;
    }

    glm::vec3 Position() const {
      return m_position;
    }

    glm::vec3 Rotation() const {
      return m_rotation;
    }

    glm::vec3 Scale() const {
      return m_scale;
    }

  public:
    const Mesh* mesh;
    const Material* material; // TODO replace with ID

  private:
    // TODO move to SceneObject
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    // TODO mutable? At least, while it's not accessible outside
    mutable glm::mat4 m_modelMatrix;
    mutable glm::mat4 m_modelMatrixInv;
    mutable bool m_modelMatrixDirty;
  };
}
