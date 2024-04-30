#pragma once
#include "Bvh.h"
#include "IHitable.h"
#include "MeshData.h"
#include "Triangle.h"

namespace Flame {
  struct Mesh final : IHitable {
    explicit Mesh(const MeshData& data)
    : triangles(CreateTriangles(data))
    , hitables(CreateHitables(triangles))
    , bvh(hitables) {
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      return bvh.Hit(r, record, tMin, tMax);
    }

  private:
    static std::vector<Triangle> CreateTriangles(const MeshData& data) {
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

    static std::vector<IHitableWithBounds*> CreateHitables(const std::vector<Triangle>& triangles) {
      std::vector<IHitableWithBounds*> hitables(triangles.size());
      for (int i = 0; i < triangles.size(); ++i) {
        hitables[i] = const_cast<Triangle*>(&triangles[i]);
      }

      return hitables;
    }

  public:
    std::vector<Triangle> triangles;
    std::vector<IHitableWithBounds*> hitables;
    Bvh bvh;

    // TODO bindings
    std::vector<MaterialBinding> materialsId;
  };
}
