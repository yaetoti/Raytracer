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
    , m_modelMatrix(glm::mat4(1.0f)) {
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      glm::vec4 origin = glm::inverse(m_modelMatrix) * glm::vec4(r.origin, 1.0f);
      glm::vec4 direction = glm::inverse(m_modelMatrix) * glm::vec4(r.direction, 0.0f);
      Ray ray(origin / origin.w, direction);

      // TODO scale time limits?
      if (mesh->Hit(ray, record, tMin, tMax)) {
        record.material = material;
        record.hitable = const_cast<MeshObject*>(this);
        // TODO move transformation into HitRecord
        glm::vec4 normal(record.normal, 0.0f);
        glm::vec4 point(record.point, 1.0f);
        record.normal = m_modelMatrix * normal;
        record.point = m_modelMatrix * point;
        return true;
      }

      return false;
    }

    void SetPosition(glm::vec3 position) {
      m_position = position;
      UpdateModelMatrix();
    }

    void SetRotation(glm::vec3 rotation) {
      m_rotation = rotation;
      UpdateModelMatrix();
    }

    void SetScale(glm::vec3 scale) {
      m_scale = scale;
      UpdateModelMatrix();
    }

    void UpdateModelMatrix() {
      m_modelMatrix = glm::translate(m_position)
      * glm::scale(m_scale)
      * glm::eulerAngleZ(m_rotation.z)
      * glm::eulerAngleX(m_rotation.x)
      * glm::eulerAngleY(m_rotation.y);
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
    glm::mat4 m_modelMatrix;
  };
}
