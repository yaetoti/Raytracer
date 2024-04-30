#pragma once
#include "IHitable.h"
#include "Mesh.h"

namespace Flame {
  struct MeshObject final : IHitable {
    MeshObject(const Mesh* mesh, const Material* material)
    : modelMatrix(glm::mat4(1.0f))
    , mesh(mesh)
    , material(material) {
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      glm::vec4 origin = glm::inverse(modelMatrix) * glm::vec4(r.origin, 1.0f);
      glm::vec4 direction = glm::inverse(modelMatrix) * glm::vec4(r.direction, 0.0f);
      Ray ray(origin / origin.w, direction);

      // TODO scale time limits?
      if (mesh->Hit(ray, record, tMin, tMax)) {
        record.material = material;
        record.hitable = const_cast<MeshObject*>(this);
        // TODO move transformation into HitRecord
        glm::vec4 normal(record.normal, 0.0f);
        glm::vec4 point(record.point, 1.0f);
        record.normal = modelMatrix * normal;
        record.point = modelMatrix * point;
        return true;
      }

      return false;
    }

  public:
    glm::mat4 modelMatrix;
    const Mesh* mesh;
    const Material* material; // TODO replace with ID
  };
}
