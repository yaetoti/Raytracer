#pragma once
#include <vector>

#include "Flame/math/IHitable.h"

namespace Flame {
  struct Scene;

  struct Face final {
    uint32_t vertice0Id;
    uint32_t vertice1Id;
    uint32_t vertice2Id;
    uint32_t normalId;
  };

  struct MaterialBinding final {
    uint32_t startFaceId;
    uint32_t materialId;
  };

  struct Mesh final : IHitable {
    bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const override {
      
    }

  public:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<Face> faces;
    std::vector<MaterialBinding> materials;
  };  
}
