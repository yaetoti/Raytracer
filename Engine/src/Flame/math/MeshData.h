#pragma once
#include <array>
#include <cstdint>
#include <ostream>
#include <vector>
#include <glm/vec3.hpp>

namespace Flame {
  struct Face final {
    Face() = default;
    explicit Face(uint32_t idV0, uint32_t idV1, uint32_t idV2, uint32_t idNormal)
    : vertices({ idV0, idV1, idV2 })
    , normal(idNormal) {
    }

    friend std::ostream& operator<<(std::ostream& os, const Face& obj) {
      return os << "f "
        << obj.vertices[0] << "//" << obj.normal << ' '
        << obj.vertices[1] << "//" << obj.normal << ' '
        << obj.vertices[2] << "//" << obj.normal;
    }

    std::array<uint32_t, 3> vertices;
    uint32_t normal;
  };

  struct MeshData final {
    friend std::ostream& operator<<(std::ostream& os, const MeshData& obj) {
      for (const auto& vertex : obj.vertices) {
        os << "v " << vertex[0] << ' ' << vertex[1] << ' ' << vertex[2] << '\n';
      }

      for (const auto& normal : obj.normals) {
        os << "n " << normal[0] << ' ' << normal[1] << ' ' << normal[2] << '\n';
      }

      for (const auto& face : obj.faces) {
        os << face << '\n';
      }

      return os;
    }

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<Face> faces;
  };

  struct MaterialBinding final {
    uint32_t startFaceId;
    uint32_t materialId;
  };
}
