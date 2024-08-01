#pragma once

#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

#include "Aabb.h"

namespace Flame {
  struct BvhNode final {
    BvhNode();

  public:
    uint32_t m_leftId;
    uint32_t m_rightId;
    Aabb m_bound;
    std::vector<uint32_t> m_boundsId;
  };

  struct Bvh final : IHitable {
    explicit Bvh(std::vector<IHitableWithBounds*>& hitables);

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override;

  private:
    void InitBounds();
    Aabb CalculateBound(const std::vector<uint32_t>& boundsId) const;
    glm::vec3 CalculateSplitPoint(const std::vector<uint32_t>& boundsId) const;
    uint32_t InitNode(std::vector<uint32_t>&& boundsId, bool shouldSubdivide = true);
    bool HitNode(uint32_t nodeId, const Ray& r, HitRecord& record, float tMin, float tMax) const;

  public:
    const std::vector<IHitableWithBounds*>& m_hitables;
    std::vector<Aabb> m_bounds;
    std::vector<BvhNode> m_nodes;
  };
}
