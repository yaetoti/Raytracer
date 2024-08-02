#include "Mesh.h"

namespace Flame {
  Mesh::Mesh(const MeshData& data)
  : triangles(CreateTriangles(data))
  , hitables(CreateHitables(triangles))
  , bvh(hitables) {
  }

  bool Mesh::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    return bvh.Hit(r, record, tMin, tMax);
  }

  std::vector<Triangle> Mesh::CreateTriangles(const MeshData& data) {
    std::vector<Triangle> triangles;
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

  std::vector<IHitableWithBounds*> Mesh::CreateHitables(const std::vector<Triangle>& triangles) {
    std::vector<IHitableWithBounds*> hitables(triangles.size());
    for (int i = 0; i < triangles.size(); ++i) {
      hitables[i] = const_cast<Triangle*>(&triangles[i]);
    }

    return hitables;
  }
}
