#pragma once

#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <assimp/vector3.h>
#include <glm/vec3.hpp>
#include "MeshBvh.h"
#include "Flame/math/Aabb.h"
#include "Flame/math/HitRecord.h"

namespace Flame {
  struct Face final {
    uint32_t indices[3];
  };

  struct Mesh final {
    Mesh()
    : bvh(this) {
    }

    bool Hit(const Ray& r, HitRecord<const Mesh*>& record, float tMin, float tMax) const {
      if (!bvh.Hit(r, record, tMin, tMax)) {
        return false;
      }

      record.data = this;
      return true;
    }

    void Parse(const aiMesh& mesh) {
      assert(sizeof(aiVector3D) == sizeof(glm::vec3));
      assert(sizeof(aiAABB) == sizeof(Aabb));
      assert(sizeof(Face) == 3 * sizeof(uint32_t));
      assert(mesh.mNormals);

      vertices.resize(mesh.mNumVertices);
      normals.resize(mesh.mNumVertices);
      uvs.resize(mesh.mNumVertices);
      faces.resize(mesh.mNumFaces);

      std::memcpy(vertices.data(), mesh.mVertices, mesh.mNumVertices * sizeof(aiVector3D));
      std::memcpy(normals.data(), mesh.mNormals, mesh.mNumVertices * sizeof(aiVector3D));
      //assert(mesh.mTextureCoords); 
      if (mesh.mTextureCoords[0]) {
        for (uint32_t i = 0; i < mesh.mNumVertices; ++i) {
          aiVector3D uv = mesh.mTextureCoords[0][i];
          uvs[i].x = uv.x;
          uvs[i].y = uv.y;
        }
      }

      for (uint32_t i = 0; i < mesh.mNumFaces; ++i) {
        std::memcpy(faces.data() + i, mesh.mFaces[i].mIndices, 3 * sizeof(uint32_t));
      }
      std::memcpy(&box, &mesh.mAABB, sizeof(aiAABB));
      name = mesh.mName.C_Str();

      bvh.Build();
    }

    void BuildBvh() {
      bvh.Build();
    }

  public:
    std::string name;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::mat4> transforms;
    std::vector<glm::mat4> transformsInv;
    std::vector<Face> faces;
    Aabb box;
    MeshBvh bvh;
  };
}


