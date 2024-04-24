#include "Triangle.h"

namespace Flame {
  Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal, const IMaterial& material)
  : point1(p1)
  , point2(p2)
  , point3(p3)
  , normal(normal)
  , material(material) {
  }

  bool Triangle::Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const {
    // Intersection with plane
    float t = glm::dot(normal, point1 - r.origin) / glm::dot(normal, r.direction);
    if (t > tMin && t < tMax) {
      glm::vec3 point = r.AtParameter(t);
      glm::vec3 v12 = point2 - point1;
      glm::vec3 v1p = point - point1;
      glm::vec3 v23 = point3 - point2;
      glm::vec3 anormal = glm::cross(v12, v23);
      glm::vec3 v2p = point - point2;
      glm::vec3 v31 = point1 - point3;
      glm::vec3 v3p = point - point3;
      if (glm::dot(anormal, glm::cross(v12, v1p)) > 0 
          && glm::dot(anormal, glm::cross(v23, v2p)) > 0
          && glm::dot(anormal, glm::cross(v31, v3p)) > 0) {
        record.point = point;
        record.normal = normal;
        record.time = t;
        record.material = &material;
        record.hitable = const_cast<Triangle*>(this);
        return true;
      }
    }

    return false;
  }
}

