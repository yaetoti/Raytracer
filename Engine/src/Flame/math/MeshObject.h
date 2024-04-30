#pragma once
#include "IHitable.h"
#include "Mesh.h"

namespace Flame {
  struct MeshObject final : IHitable {
    MeshObject(const Mesh* mesh, const Material* material)
    : mesh(mesh)
    , material(material) {
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      // TODO transform ray
      // TODO scale time?
      if (mesh->Hit(r, record, tMin, tMax)) {
        record.material = material;
        record.hitable = const_cast<MeshObject*>(this);
        return true;
      }

      return false;
    }

  public:
    const Mesh* mesh;
    const Material* material; // TODO replace with ID
  };
}
