#include "MeshBvh.h"

#include <iostream>
#include <numeric>

#include "Mesh.h"
#include "Flame/math/MathUtils.h"

namespace Flame {
  MeshBvh::MeshBvhNode::MeshBvhNode()
  : leftId(std::numeric_limits<uint32_t>::max())
  , rightId(std::numeric_limits<uint32_t>::max()) {

  }

  MeshBvh::MeshBvh(const Mesh* mesh)
  : m_mesh(mesh) {
  }

  void MeshBvh::Build() {
    assert(m_mesh->faces.size() != 0);
    m_boxes.clear();
    m_nodes.clear();

    InitBounds();
    m_nodes.reserve(m_boxes.size() * 2 - 1);

    // Indices of faces
    std::vector<uint32_t> indices(m_boxes.size());
    std::iota(indices.begin(), indices.end(), 0);
    InitNodes(std::move(indices));

    m_nodes.shrink_to_fit();
  }

  bool MeshBvh::Hit(const Ray& r, HitRecord<const Mesh*>& record, float tMin, float tMax) const {
    return HitNode(0, r, record, tMin, tMax);
  }

  bool MeshBvh::HitFace(uint32_t faceId, const Ray& r, HitRecord<const Mesh*>& record, float tMin, float tMax) const {
    glm::vec3 vertices[3] = {
      m_mesh->vertices[m_mesh->faces[faceId].indices[0]],
      m_mesh->vertices[m_mesh->faces[faceId].indices[1]],
      m_mesh->vertices[m_mesh->faces[faceId].indices[2]],
    };

    glm::vec3 v0v1 = vertices[1] - vertices[0];
	  glm::vec3 v0v2 = vertices[2] - vertices[0];

	  glm::vec3 pvec = glm::cross(r.direction, v0v2);
	  float det = glm::dot(v0v1, pvec);
	  if (det < 0.00001) {
		  return false;
	  }

	  float invDet = 1.0f / det;
	  glm::vec3 tvec = r.origin - vertices[0];

	  float u = glm::dot(tvec, pvec) * invDet;
	  if (u < 0 || u > 1) {
		  return false;
	  }

	  glm::vec3 qvec = glm::cross(tvec, v0v1);
	  float v = glm::dot(r.direction, qvec) * invDet;
	  if (v < 0 || u + v > 1) {
		  return false;
	  }

	  float t = glm::dot(v0v2, qvec) * invDet;
    if (t > tMin && t < tMax) {
      glm::vec3 normals[3] = {
        m_mesh->normals[m_mesh->faces[faceId].indices[0]],
        m_mesh->normals[m_mesh->faces[faceId].indices[1]],
        m_mesh->normals[m_mesh->faces[faceId].indices[2]],
      };
      glm::vec3 barycentric = MathUtils::ToBarycentric(record.point, vertices);

      record.time = t;
      record.point = r.AtParameter(t);
      // Interpolate normal
      //record.normal = barycentric.x * normals[0] + barycentric.y * normals[1] + barycentric.z * normals[2];
      record.normal = glm::cross(v0v1, v0v2);
	    return true;
    }

	  return false;
  }

  bool MeshBvh::HitNode(uint32_t nodeId, const Ray& r, HitRecord<const Mesh*>& record, float tMin, float tMax) const {
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

    const MeshBvhNode& node = m_nodes[nodeId];

    // If bound wasn't hit - no hit
    {
      HitRecord<const Aabb*> record0;
      if (!node.box.Hit(r, record0, tMin, tMax)) {
        return false;
      }
    }

    // If leaf
    if (!node.boxesId.empty()) {
      HitRecord<const Mesh*> record0;
      record0.time = tMax;
      bool anyHit = false;
      for (uint32_t id : node.boxesId) {
        if (HitFace(id, r, record0, tMin, record0.time)) {
          anyHit = true;
        }
      }

      if (anyHit) {
        record = record0;
        return true;
      }

      return false;
    }

    // If compound
    HitRecord<const Mesh*> record0;
    record0.time = tMax;
    bool anyHit = HitNode(node.leftId, r, record0, tMin, record0.time);
    anyHit |= HitNode(node.rightId, r, record0, tMin, record0.time);
    if (anyHit) {
      record = record0;
      return true;
    }

    return false;
  }

  void MeshBvh::InitBounds() {
    // Create boxes for triangles
    m_boxes.reserve(m_mesh->faces.size() * 2 - 1);
    for (uint32_t i = 0; i < m_mesh->faces.size(); ++i) {
      m_boxes.emplace_back(GetFaceBounds(i));
    }
  }

  uint32_t MeshBvh::InitNodes(std::vector<uint32_t>&& boundsId, bool shouldSubdivide) {
    uint32_t currentIndex = static_cast<uint32_t>(m_nodes.size());
    MeshBvhNode& node = m_nodes.emplace_back();
    node.box = CalculateBound(boundsId);

    if (!shouldSubdivide || boundsId.size() == 1) {
      node.boxesId = std::move(boundsId);
      return currentIndex;
    }

    glm::vec3 splitPoint = CalculateSplitPoint(boundsId);
    uint32_t splitAxis = node.box.GetBiggestSideIndex();

    // Determine affiliation
    std::vector<uint32_t> indicesLeft;
    std::vector<uint32_t> indicesRight;
    for (uint32_t id : boundsId) {
      // Centroids are better than min/max because the farthest arises 2 problems:
      // - 2 children may return the same element
      // - Since we calculate TheBigDanny in constructor, both children will have bloated volume
      if (m_boxes[id].Centroid()[splitAxis] < splitPoint[splitAxis]) {
        // Left may be zero length if all triangles have the same centroid because of <
        indicesLeft.emplace_back(id);
      } else {
        indicesRight.emplace_back(id);
      }
    }

    if ((indicesLeft.size() == boundsId.size() && indicesRight.size() == boundsId.size())
      || indicesLeft.empty() || indicesRight.empty()) {
      // Can't subdivide further
      node.boxesId = std::move(boundsId);
      return currentIndex;
    }

    // Don't subdivide children if elements count is the same
    node.leftId = InitNodes(std::move(indicesLeft), indicesLeft.size() != boundsId.size());
    node.rightId = InitNodes(std::move(indicesRight), indicesRight.size() != boundsId.size());

    return currentIndex;
  }

  Aabb MeshBvh::GetFaceBounds(uint32_t id) const {
    glm::vec3 vertices[3] = {
      m_mesh->vertices[m_mesh->faces[id].indices[0]],
      m_mesh->vertices[m_mesh->faces[id].indices[1]],
      m_mesh->vertices[m_mesh->faces[id].indices[2]],
    };
    return Aabb::Union(vertices, vertices + 3);
  }

  Aabb MeshBvh::CalculateBound(const std::vector<uint32_t>& boundsId) const {
    glm::vec3 min(std::numeric_limits<float>::infinity());
    glm::vec3 max(-std::numeric_limits<float>::infinity());

    // Calculate The Big Danny
    for (uint32_t id : boundsId) {
      for (int dim = 0; dim < 3; ++dim) {
        min[dim] = glm::min(m_boxes[id].Min()[dim], min[dim]);
        max[dim] = glm::max(m_boxes[id].Max()[dim], max[dim]);
      }
    }

    return Aabb(min, max);
  }

  glm::vec3 MeshBvh::CalculateSplitPoint(const std::vector<uint32_t>& boundsId) const {
    // Find average centroid
    glm::vec3 avgCentroid(0.0f);
    for (uint32_t id : boundsId) {
      avgCentroid += m_boxes[id].Centroid();
    }

    return avgCentroid / boundsId.size();
  }
}
