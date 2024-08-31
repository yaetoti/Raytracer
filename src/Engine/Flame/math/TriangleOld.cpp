#include "TriangleOld.h"

#include "MathUtils.h"

namespace Flame {
  TriangleOld::TriangleOld(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 normal)
  : points { point1, point2, point3 }
  , normal(normal) {
    bound = MathUtils::AabbFromTriangle(*this);
  }

  bool TriangleOld::Hit(const Ray& r, HitRecordOld& record, float tMin, float tMax) const {
#if 0
    // Is parallel
    if (fabs(glm::dot(normal, r.direction)) <= 0.00001f) {
      return false;
    }

    float t = glm::dot(normal, points[0] - r.origin) / glm::dot(normal, r.direction);
    if (t > tMin && t < tMax) {
      glm::vec3 point = r.AtParameter(t);
      glm::vec3 v12 = points[1] - points[0];
      glm::vec3 v1p = point - points[0];
      glm::vec3 v23 = points[2] - points[1];
      glm::vec3 v2p = point - points[1];
      glm::vec3 v31 = points[0] - points[2];
      glm::vec3 v3p = point - points[2];
      // glm::vec3 calcNormal = glm::cross(v12, v23);
      glm::vec3 calcNormal = normal;
      if (glm::dot(calcNormal, glm::cross(v12, v1p)) > 0
        && glm::dot(calcNormal, glm::cross(v23, v2p)) > 0
        && glm::dot(calcNormal, glm::cross(v31, v3p)) > 0) {
        record.point = point;
        record.normal = normal;
        record.time = t;
        return true;
      }
    }

    return false;
#else
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
#endif
  }
}
