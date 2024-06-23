#pragma once
#include "Bvh.h"
#include "IHitable.h"
#include "MeshData.h"
#include "Triangle.h"

namespace Flame {
  struct Mesh final : IHitable {
    explicit Mesh(const MeshData& data);

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

  private:
    static std::vector<Triangle> CreateTriangles(const MeshData& data);
    static std::vector<IHitableWithBounds*> CreateHitables(const std::vector<Triangle>& triangles);

  public:
    std::vector<Triangle> triangles;
    std::vector<IHitableWithBounds*> hitables;
    Bvh bvh;

    // TODO bindings
    std::vector<MaterialBinding> materialsId;
  };
}
