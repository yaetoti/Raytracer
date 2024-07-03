#include "Bvh.h"

namespace Flame {
  BvhNode::BvhNode()
  : m_leftId(std::numeric_limits<uint32_t>::max())
  , m_rightId(std::numeric_limits<uint32_t>::max()) {
  }

  Bvh::Bvh(std::vector<IHitableWithBounds*>& hitables): m_hitables(hitables) {
    InitBounds();
    // Reminder: program will crash if size == 0 => reserveSize == UINT_MAX.
    m_nodes.reserve(m_hitables.size() * 2 - 1);
       
    std::vector<uint32_t> indices(m_hitables.size());
    std::iota(indices.begin(), indices.end(), 0);
    InitNode(std::move(indices));

    m_nodes.shrink_to_fit();
  }

  bool Bvh::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    return HitNode(0, r, record, tMin, tMax);
  }

  void Bvh::InitBounds() {
    // Create boxes for triangles
    m_bounds.reserve(m_hitables.size() * 2 - 1);
    for (const auto& hitable : m_hitables) {
      m_bounds.emplace_back(hitable->GetBound());
    }
  }

  Aabb Bvh::CalculateBound(const std::vector<uint32_t>& boundsId) const {
    glm::vec3 min(std::numeric_limits<float>::infinity());
    glm::vec3 max(-std::numeric_limits<float>::infinity());

    // Calculate The Big Danny
    for (uint32_t id : boundsId) {
      for (int dim = 0; dim < 3; ++dim) {
        min[dim] = glm::min(m_bounds[id].Min()[dim], min[dim]);
        max[dim] = glm::max(m_bounds[id].Max()[dim], max[dim]);
      }
    }

    return Aabb(min, max);
  }

  glm::vec3 Bvh::CalculateSplitPoint(const std::vector<uint32_t>& boundsId) const {
    // Find average centroid
    glm::vec3 avgCentroid(0.0f);
    for (uint32_t id : boundsId) {
      avgCentroid += m_bounds[id].Centroid();
    }

    return avgCentroid / boundsId.size();
  }

  uint32_t Bvh::InitNode(std::vector<uint32_t>&& boundsId, bool shouldSubdivide) {
    uint32_t currentIndex = static_cast<uint32_t>(m_nodes.size());
    BvhNode& node = m_nodes.emplace_back();
    node.m_bound = CalculateBound(boundsId);

    if (!shouldSubdivide || boundsId.size() == 1) {
      node.m_boundsId = std::move(boundsId);
      return currentIndex;
    }

    glm::vec3 splitPoint = CalculateSplitPoint(boundsId);
    uint32_t splitAxis = node.m_bound.GetBiggestSideIndex();

    // Determine affiliation
    std::vector<uint32_t> indicesLeft;
    std::vector<uint32_t> indicesRight;
    for (uint32_t id : boundsId) {
      // Centroids are better than min/max because the farthest arises 2 problems:
      // - 2 children may return the same element
      // - Since we calculate TheBigDanny in constructor, both children will have bloated volume
      if (m_bounds[id].Centroid()[splitAxis] < splitPoint[splitAxis]) {
        // Left may be zero length if all triangles have the same centroid because of <
        indicesLeft.emplace_back(id);
      } else {
        indicesRight.emplace_back(id);
      }
    }

    if ((indicesLeft.size() == boundsId.size() && indicesRight.size() == boundsId.size())
      || indicesLeft.empty() || indicesRight.empty()) {
      // Can't subdivide further
      node.m_boundsId = std::move(boundsId);
      return currentIndex;
    }

    // Don't subdivide children if elements count is the same
    node.m_leftId = InitNode(std::move(indicesLeft), indicesLeft.size() != boundsId.size());
    node.m_rightId = InitNode(std::move(indicesRight), indicesRight.size() != boundsId.size());

    return currentIndex;
  }

  bool Bvh::HitNode(uint32_t nodeId, const Ray& r, HitRecord& record, float tMin, float tMax) const {
    // BVH hit algorithm:
    // 1. Determine if current box is even being hit
    // 1.1.If not - return false
    // 1.2 If yes: ->2/3
    // 2. If current object is a leaf - determine if underlying IHitable is being hit
    // 2.1 If not - return false
    // 2.2 If yes - return true, time OF THE CLOSEST
    // 3. If current Object is a compound - call Hit() for children
    // 3.1 If none was hit - return false
    // 3.2 If one was hit - return true and its record
    // 3.3 If both were hit - compare time and return true and record of the closest one

    const BvhNode& node = m_nodes[nodeId];

    // If bound wasn't hit - no hit
    {
      HitRecord record0;
      if (!node.m_bound.Hit(r, record0, tMin, tMax)) {
        return false;
      }
    }

    // If leaf
    if (!node.m_boundsId.empty()) {
      HitRecord record0 { .time = tMax };
      bool anyHit = false;
      for (uint32_t id : node.m_boundsId) {
        if (m_hitables[id]->Hit(r, record0, tMin, record0.time)) {
          record = record0;
          anyHit = true;
        }
      }

      return anyHit;
    }

    // If compound
    HitRecord record0 { .time = tMax };
    bool anyHit = HitNode(node.m_leftId, r, record0, tMin, record0.time);
    anyHit |= HitNode(node.m_rightId, r, record0, tMin, record0.time);
    if (anyHit) {
      record = record0;
      return true;
    }

    return false;
  }
}
