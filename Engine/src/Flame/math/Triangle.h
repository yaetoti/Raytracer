#pragma once

#include <array>

#include "Aabb.h"

namespace Flame {
  struct Triangle final : IHitableWithBounds {
    explicit Triangle(
      glm::vec3 point1 = glm::vec3(0.0f),
      glm::vec3 point2 = glm::vec3(0.0f),
      glm::vec3 point3 = glm::vec3(0.0f),
      glm::vec3 normal = glm::vec3(0.0f)
    )
    : points { point1, point2, point3 }
    , normal(normal) {
      glm::vec3 min(std::numeric_limits<float>::infinity());
      glm::vec3 max(-std::numeric_limits<float>::infinity());

      for (int point = 0; point < 3; ++point) {
        for (int dim = 0; dim < 3; ++dim) {
          min[dim] = glm::min(points[point][dim], min[dim]);
          max[dim] = glm::max(points[point][dim], max[dim]);
        }
      }

      bound = Aabb(min, max);
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin = 0, float tMax = std::numeric_limits<float>::infinity()) const override {
      // Is parallel
      //if (fabs(glm::dot(normal, r.direction)) <= 0.00001f) {
      //  return false;
      //}

      //float t = glm::dot(normal, points[0] - r.origin) / glm::dot(normal, r.direction);
      //if (t > tMin && t < tMax) {
      //  glm::vec3 point = r.AtParameter(t);
      //  glm::vec3 v12 = points[1] - points[0];
      //  glm::vec3 v1p = point - points[0];
      //  glm::vec3 v23 = points[2] - points[1];
      //  glm::vec3 v2p = point - points[1];
      //  glm::vec3 v31 = points[0] - points[2];
      //  glm::vec3 v3p = point - points[2];
      //  // TODO replace with mesh data
      //  glm::vec3 calcNormal = glm::cross(v12, v23);
      //  if (glm::dot(calcNormal, glm::cross(v12, v1p)) > 0
      //      && glm::dot(calcNormal, glm::cross(v23, v2p)) > 0
      //      && glm::dot(calcNormal, glm::cross(v31, v3p)) > 0) {
      //    record.point = point;
      //    record.normal = normal;
      //    record.time = t;
      //    return true;
      //  }
      //}

      //return false;

      glm::vec3 v0v1 = points[1] - points[0];
	    glm::vec3 v0v2 = points[2] - points[0];

	    glm::vec3 pvec = glm::cross(r.direction, v0v2);
	    float det = glm::dot(v0v1, pvec);
	    if (det < 0.00001) {
		    return false;
	    }

	    float invDet = 1.0f / det;
	    glm::vec3 tvec = r.origin - points[0];

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
        record.time = t;
        record.point = r.AtParameter(t);
        record.normal = normal;
	      return true;
      }

	    return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const Triangle& obj) {
      return os << "Triangle { "
      << obj.points[0] << ", "
      << obj.points[1] << ", "
      << obj.points[2] << ", "
      << obj.normal << " }\n";
    }

    const Aabb& GetBound() const override {
      return bound;
    }

  public:
    std::array<glm::vec3, 3> points;
    glm::vec3 normal;
    Aabb bound;
  };

  inline Aabb GetTriangleBox(const Triangle& o) {
    glm::vec3 min(std::numeric_limits<float>::infinity());
    glm::vec3 max(-std::numeric_limits<float>::infinity());
    for (int point = 0; point < 3; ++point) {
      for (int dim = 0; dim < 3; ++dim) {
        min[dim] = glm::min(o.points[point][dim], min[dim]);
        max[dim] = glm::max(o.points[point][dim], max[dim]);
      }
    }

    return Aabb(min, max);
  }
}
