#include "TriangleObject.h"

namespace Flame {
  TriangleObject::TriangleObject(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal, int materialId)
  : point1(p1)
  , point2(p2)
  , point3(p3)
  , normal(normal)
  , materialId(materialId) {
  }

  bool TriangleObject::Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    glm::vec3 edge1 = point2 - point1;
    glm::vec3 edge2 = point3 - point1;
    glm::vec3 rayCrossE2 = glm::cross(r.direction, edge2);
    float det = glm::dot(edge1, rayCrossE2);
    if (det > -epsilon && det < epsilon) {
      // Ray is parallel to the triangle.
      return false;
    }

    float invDet = 1.0f / det;
    glm::vec3 s = r.origin - point1;
    float u = invDet * glm::dot(s, rayCrossE2);
    if (u < 0.0f || u > 1.0f) {
      return false;
    }

    glm::vec3 sCrossE1 = glm::cross(s, edge1);
    float v = invDet * glm::dot(r.direction, sCrossE1);
    if (v < 0 || u + v > 1) {
        return false;
    }

    float t = invDet * glm::dot(edge2, sCrossE1);
    if (t > tMin && t < tMax) {
      record.point = r.AtParameter(t);
      record.normal = normal;
      record.time = t;
      record.materialId = materialId;
      record.hitable = const_cast<TriangleObject*>(this);
      return true;
    }

    return false;

#if 0
    // Is parallel
    if (fabs(glm::dot(normal, r.direction)) <= 0.01f) {
      return false;
    }

    float t = glm::dot(normal, point1 - r.origin) / glm::dot(normal, r.direction);
    if (t > tMin && t < tMax) {
      glm::vec3 point = r.AtParameter(t);
      glm::vec3 v12 = point2 - point1;
      glm::vec3 v1p = point - point1;
      glm::vec3 v23 = point3 - point2;
      glm::vec3 v2p = point - point2;
      glm::vec3 v31 = point1 - point3;
      glm::vec3 v3p = point - point3;
      // TODO replace with mesh data
      glm::vec3 calcNormal = glm::cross(v12, v23);
      if (glm::dot(calcNormal, glm::cross(v12, v1p)) > 0
          && glm::dot(calcNormal, glm::cross(v23, v2p)) > 0
          && glm::dot(calcNormal, glm::cross(v31, v3p)) > 0) {
        record.point = point;
        record.normal = normal;
        record.time = t;
        record.materialId = &materialId;
        record.hitable = const_cast<TriangleObject*>(this);
        return true;
      }
    }

    return false;
#endif
  }
}

