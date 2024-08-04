#pragma once
#include <cstdint>
#include <vector>

#include "Flame/math/Aabb.h"
#include "Flame/math/Ray.h"

namespace Flame {
  struct Mesh;

  struct MeshBvh final {
    struct MeshBvhNode final {
      MeshBvhNode();

      uint32_t leftId;
      uint32_t rightId;
      Aabb box;
      std::vector<uint32_t> boxesId;
    };

    explicit MeshBvh(const Mesh* mesh);

    void Build();
    bool Hit(const Ray& r, HitRecord<const Mesh*>& record, float tMin, float tMax) const;

  private:
    // Really really hard
    bool HitFace(uint32_t faceId, const Ray& r, HitRecord<const Mesh*>& record, float tMin, float tMax) const;
    bool HitNode(uint32_t nodeId, const Ray& r, HitRecord<const Mesh*>& record, float tMin, float tMax) const;

    void InitBounds();
    uint32_t InitNodes(std::vector<uint32_t>&& boundsId, bool shouldSubdivide = true);
    Aabb GetFaceBounds(uint32_t id) const;
    Aabb CalculateBound(const std::vector<uint32_t>& boundsId) const;
    glm::vec3 CalculateSplitPoint(const std::vector<uint32_t>& boundsId) const;

  private:
    const Mesh* m_mesh;
    std::vector<Aabb> m_boxes;
    std::vector<MeshBvhNode> m_nodes;
  };
}
