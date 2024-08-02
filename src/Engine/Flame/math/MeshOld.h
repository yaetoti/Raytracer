#pragma once
#include "BvhOld.h"
#include "IHitable.h"
#include "MeshData.h"
#include "TriangleOld.h"

namespace Flame {
  struct MeshOld final : IHitable {
    explicit MeshOld(const MeshData& data);

    bool Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const override;

  private:
    static std::vector<TriangleOld> CreateTriangles(const MeshData& data);
    static std::vector<IHitableWithBounds*> CreateHitables(const std::vector<TriangleOld>& triangles);

  public:
    std::vector<TriangleOld> triangles;
    std::vector<IHitableWithBounds*> hitables;
    BvhOld bvh;

    // TODO bindings
    std::vector<MaterialBinding> materialsId;
  };
}
