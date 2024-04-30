#pragma once

#include "IHitable.h"
#include "TriangleObject.h"
#include <vector>

#include "MathUtils.h"

namespace Flame {
  struct CubeFace final : IHitable {
    CubeFace(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 normal, const Material* material)
    : half1(p1, p2, p3, normal, material)
    , half2(p1, p3, p4, normal, material) {
    }

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      std::vector<IHitable*> hitables(2);
      hitables[0] = const_cast<TriangleObject*>(&half1);
      hitables[1] = const_cast<TriangleObject*>(&half2);

      return MathUtils::HitClosest(hitables.begin(), hitables.end(), r, tMin, tMax, record);
    }

    TriangleObject half1;
    TriangleObject half2;
  };

  struct Cube final : IHitable {
    // TODO WTF
    explicit Cube(const Material* material)
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

    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      std::vector<IHitable*> hitables(6);
      hitables[0] = const_cast<CubeFace*>(&top);
      hitables[1] = const_cast<CubeFace*>(&bottom);
      hitables[2] = const_cast<CubeFace*>(&left);
      hitables[3] = const_cast<CubeFace*>(&right);
      hitables[4] = const_cast<CubeFace*>(&faceFar);
      hitables[5] = const_cast<CubeFace*>(&faceNear);

      return MathUtils::HitClosest(hitables.begin(), hitables.end(), r, tMin, tMax, record);
    }

    CubeFace top;
    CubeFace bottom;
    CubeFace left;
    CubeFace right;
    CubeFace faceFar;
    CubeFace faceNear;
  };
}
