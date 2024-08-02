#pragma once

#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

#include "AabbOld.h"

namespace Flame {
  struct BvhNodeOld final {
    BvhNodeOld();

  public:
    uint32_t m_leftId;
    uint32_t m_rightId;
    AabbOld m_bound;
    std::vector<uint32_t> m_boundsId;
  };

  struct BvhOld final : IHitable {
    explicit BvhOld(std::vector<IHitableWithBounds*>& hitables);

    bool Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const override;

  private:
    void InitBounds();
    AabbOld CalculateBound(const std::vector<uint32_t>& boundsId) const;
    glm::vec3 CalculateSplitPoint(const std::vector<uint32_t>& boundsId) const;
    uint32_t InitNode(std::vector<uint32_t>&& boundsId, bool shouldSubdivide = true);
    bool HitNode(uint32_t nodeId, const Ray& r, HitRecordOld& record, float tMin, float tMax) const;

  public:
    const std::vector<IHitableWithBounds*>& m_hitables;
    std::vector<AabbOld> m_bounds;
    std::vector<BvhNodeOld> m_nodes;
  };
}
