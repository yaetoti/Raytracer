#pragma once

#include "IHitable.h"
#include "Triangle.h"
#include <vector>

#include "MathUtils.h"

namespace Flame {
  struct Face final : IHitable {
    Face(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 normal, const IMaterial& material)
    : half1(p1, p2, p3, normal, material)
    , half2(p1, p3, p4, normal, material) {
    }

    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const override {
      std::vector<IHitable*> hitables(2);
      hitables[0] = const_cast<Triangle*>(&half1);
      hitables[1] = const_cast<Triangle*>(&half2);

      return MathUtils::HitClosest(hitables.begin(), hitables.end(), r, tMin, tMax, record);
    }

    Triangle half1;
    Triangle half2;
  };

  struct Cube final : IHitable {
    // TODO WTF
    explicit Cube(const IMaterial& material)
    : top(
      glm::vec3(-1, 1, 1),
      glm::vec3(1, 1, 1),
      glm::vec3(1, 1, -1),
      glm::vec3(-1, 1, -1),
      glm::vec3(0, 1, 0),
      material
    )
    , bottom(
      glm::vec3(-1, -1, 1),
      glm::vec3(1, -1, 1),
      glm::vec3(1, -1, -1),
      glm::vec3(-1, -1, -1),
      glm::vec3(0, -1, 0),
      material
    )
    , left(
      glm::vec3(-1, -1, 1),
      glm::vec3(-1, -1, -1),
      glm::vec3(-1, 1, -1),
      glm::vec3(-1, 1, 1),
      glm::vec3(-1, 0, 0),
      material
    )
    , right(
      glm::vec3(1, 1, -1),
      glm::vec3(1, -1, -1),
      glm::vec3(1, -1, 1),
      glm::vec3(1, 1, 1),
      glm::vec3(1, 0, 0),
      material
    )
    , faceFar(
      glm::vec3(-1, 1, 1),
      glm::vec3(1, 1, 1),
      glm::vec3(1, -1, 1),
      glm::vec3(-1, -1, 1),
      glm::vec3(0, 0, 1),
      material
    )
    , faceNear(
      glm::vec3(-1, 1, -1),
      glm::vec3(1, 1, -1),
      glm::vec3(1, -1, -1),
      glm::vec3(-1, -1, -1),
      glm::vec3(0, 0, -1),
      material
    ) {
    }

    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& record) const override {
      std::vector<IHitable*> hitables(6);
      hitables[0] = const_cast<Face*>(&top);
      hitables[1] = const_cast<Face*>(&bottom);
      hitables[2] = const_cast<Face*>(&left);
      hitables[3] = const_cast<Face*>(&right);
      hitables[4] = const_cast<Face*>(&faceFar);
      hitables[5] = const_cast<Face*>(&faceNear);

      return MathUtils::HitClosest(hitables.begin(), hitables.end(), r, tMin, tMax, record);
    }

    Face top;
    Face bottom;
    Face left;
    Face right;
    Face faceFar;
    Face faceNear;
  };
}
