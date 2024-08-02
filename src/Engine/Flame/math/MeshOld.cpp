#include "MeshOld.h"

namespace Flame {
  MeshOld::MeshOld(const MeshData& data)
  : triangles(CreateTriangles(data))
  , hitables(CreateHitables(triangles))
  , bvh(hitables) {
  }

  bool MeshOld::Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const {
    return bvh.Hit(r, record, tMin, tMax);
  }

  std::vector<TriangleOld> MeshOld::CreateTriangles(const MeshData& data) {
    std::vector<TriangleOld> triangles;
    triangles.reserve(data.faces.size());

    // Fill start data
    for (const auto& face : data.faces) {
      triangles.emplace_back(
        data.vertices[face.vertices[0]],
        data.vertices[face.vertices[1]],
        data.vertices[face.vertices[2]],
        data.normals[face.normal]
      );
    }

    return triangles;
  }

  std::vector<IHitableWithBounds*> MeshOld::CreateHitables(const std::vector<TriangleOld>& triangles) {
    std::vector<IHitableWithBounds*> hitables(triangles.size());
    for (int i = 0; i < triangles.size(); ++i) {
      hitables[i] = const_cast<TriangleOld*>(&triangles[i]);
    }

    return hitables;
  }
}
