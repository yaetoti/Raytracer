#include "ModelManager.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <memory>

namespace Flame {
  std::shared_ptr<Model> ModelManager::GetModel(const std::string& path) {
    if (!LoadModel(path)) {
      return nullptr;
    }

    return m_models.at(path);
  }

  std::shared_ptr<Model> ModelManager::GetBuiltinModel(BuiltinModelType type) {
    return m_builtinModels[static_cast<uint32_t>(type)];
  }

  bool ModelManager::LoadModel(const std::string& path) {
    if (m_models.contains(path)) {
      return true;
    }

    auto scene = m_importer.ReadFile(path.c_str(), kLoadFlags);
    assert(scene && (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 1);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
      return false;
    }

    auto model = std::make_shared<Model>();
    model->Parse(*scene);
    m_models.emplace(path, std::move(model));
    m_importer.FreeScene();

    return true;
  }

  void ModelManager::Init() {
    m_builtinModels.resize(static_cast<uint32_t>(BuiltinModelType::COUNT));

    m_builtinModels[uint32_t(BuiltinModelType::UNIT_SPHERE)] = GenerateUnitSphereModel();
    //m_builtinModels[uint32_t(BuiltinModelType::UNIT_SPHERE_FLAT)] = GenerateFlatUnitSphereModel();
  }

  void ModelManager::Cleanup() {
    m_models.clear();
    m_builtinModels.clear();
  }

  ModelManager* ModelManager::Get() {
    static ModelManager instance;
    return &instance;
  }

  std::shared_ptr<Model> ModelManager::GenerateUnitSphereModel() const {
    const uint32_t SIDES = 6;
    const uint32_t GRID_SIZE = 12;
    const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
    const uint32_t VERT_PER_SIZE = (GRID_SIZE + 1) * (GRID_SIZE + 1);

    auto model = std::make_shared<Model>();
    Mesh& mesh = model->m_meshes.emplace_back();
    mesh.name = "UNIT_SPHERE";

    mesh.vertices.resize(VERT_PER_SIZE * SIDES);
    mesh.normals.resize(VERT_PER_SIZE * SIDES);
    mesh.tangents.resize(VERT_PER_SIZE * SIDES);
    mesh.bitangents.resize(VERT_PER_SIZE * SIDES);
    mesh.uvs.resize(VERT_PER_SIZE * SIDES);

    int sideMasks[6][3] = {
      { 2, 1, 0 },
      { 0, 1, 2 },
      { 2, 1, 0 },
      { 0, 1, 2 },
      { 0, 2, 1 },
      { 0, 2, 1 }
    };

    float sideSigns[6][3] = {
      { +1, +1, +1 },
      { -1, +1, +1 },
      { -1, +1, -1 },
      { +1, +1, -1 },
      { +1, -1, -1 },
      { +1, +1, +1 }
    };

    auto* position = mesh.vertices.data();
    auto* normal = mesh.normals.data();

    for (int side = 0; side < SIDES; ++side) {
      for (int row = 0; row < GRID_SIZE + 1; ++row) {
        for (int col = 0; col < GRID_SIZE + 1; ++col) {
          glm::vec3 v;
          v.x = col / float(GRID_SIZE) * 2.f - 1.f;
          v.y = row / float(GRID_SIZE) * 2.f - 1.f;
          v.z = 1.f;

          position[0] = glm::vec3(0);
          position[0][sideMasks[side][0]] = v.x * sideSigns[side][0];
          position[0][sideMasks[side][1]] = v.y * sideSigns[side][1];
          position[0][sideMasks[side][2]] = v.z * sideSigns[side][2];
          normal[0] = position[0] = glm::normalize(position[0]);

          ++position;
          ++normal;
        }
      }
    }

    mesh.faces.resize(TRIS_PER_SIDE * SIDES);
    auto* face = mesh.faces.data();

    for (int side = 0; side < SIDES; ++side) {
      uint32_t sideOffset = VERT_PER_SIZE * side;

      for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
          face[0].indices[0] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 0;
          face[0].indices[1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
          face[0].indices[2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

          face[1].indices[0] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
          face[1].indices[1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 1;
          face[1].indices[2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

          face += 2;
        }
      }
    }

    mesh.BuildBvh();
    model->GenerateRanges();
    model->FillBuffers();
    return model;
  }

  std::shared_ptr<Model> ModelManager::GenerateFlatUnitSphereModel() const {
    const uint32_t SIDES = 6;
    const uint32_t GRID_SIZE = 12;
    const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
    const uint32_t VERT_PER_SIZE = 3 * TRIS_PER_SIDE;

    auto model = std::make_shared<Model>();
    Mesh& mesh = model->m_meshes.emplace_back();
    mesh.name = "UNIT_SPHERE_FLAT";

    mesh.vertices.resize(VERT_PER_SIZE * SIDES);
    mesh.normals.resize(VERT_PER_SIZE * SIDES);
    mesh.tangents.resize(VERT_PER_SIZE * SIDES);
    mesh.bitangents.resize(VERT_PER_SIZE * SIDES);
    mesh.uvs.resize(VERT_PER_SIZE * SIDES);

    int sideMasks[6][3] = {
      { 2, 1, 0 },
      { 0, 1, 2 },
      { 2, 1, 0 },
      { 0, 1, 2 },
      { 0, 2, 1 },
      { 0, 2, 1 }
    };

    float sideSigns[6][3] = {
      { +1, +1, +1 },
      { -1, +1, +1 },
      { -1, +1, -1 },
      { +1, +1, -1 },
      { +1, -1, -1 },
      { +1, +1, +1 }
    };

    auto* position = mesh.vertices.data();
    auto* normal = mesh.normals.data();

    for (int side = 0; side < SIDES; ++side) {
      for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
          float left = (col + 0) / float(GRID_SIZE) * 2.f - 1.f;
          float right = (col + 1) / float(GRID_SIZE) * 2.f - 1.f;
          float bottom = (row + 0) / float(GRID_SIZE) * 2.f - 1.f;
          float top = (row + 1) / float(GRID_SIZE) * 2.f - 1.f;

          glm::vec3 quad[4] = {
            { left, bottom, 1.f },
            { left, top, 1.f },
            { right, bottom, 1.f },
            { right, top, 1.f }
          };

          position[0] = position[1] = position[2] = position[3] = glm::vec3(0);
          normal[0] = normal[1] = normal[2] = normal[3] = glm::vec3(0);

          auto setPos = [sideMasks, sideSigns](int side, glm::vec3& dst, const glm::vec3& pos) {
            dst[sideMasks[side][0]] = pos.x * sideSigns[side][0];
            dst[sideMasks[side][1]] = pos.y * sideSigns[side][1];
            dst[sideMasks[side][2]] = pos.z * sideSigns[side][2];
            dst = glm::normalize(dst);
          };

          setPos(side, position[0], quad[0]);
          setPos(side, position[1], quad[1]);
          setPos(side, position[2], quad[2]);

          {
            glm::vec3 AB = position[1] - position[0];
            glm::vec3 AC = position[2] - position[0];
            normal[0] = normal[1] = normal[2] = glm::normalize(glm::cross(AB, AC));
          }

          position += 3;
          normal += 3;

          setPos(side, position[0], quad[1]);
          setPos(side, position[1], quad[3]);
          setPos(side, position[2], quad[2]);

          {
            glm::vec3 AB = position[1] - position[0];
            glm::vec3 AC = position[2] - position[0];
            normal[0] = normal[1] = normal[2] = glm::normalize(glm::cross(AB, AC));
          }

          position += 3;
          normal += 3;
        }
      }
    }

    // TODO not working
    // mesh.faces.resize(TRIS_PER_SIDE * SIDES);
    // auto* face = mesh.faces.data();

    // for (int side = 0; side < SIDES; ++side) {
    //   uint32_t sideOffset = VERT_PER_SIZE * side;

    //   for (int row = 0; row < GRID_SIZE; ++row) {
    //     for (int col = 0; col < GRID_SIZE; ++col) {
    //       face[0].indices[0] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 0;
    //       face[0].indices[1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
    //       face[0].indices[2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

    //       face[1].indices[0] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
    //       face[1].indices[1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 1;
    //       face[1].indices[2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

    //       face += 2;
    //     }
    //   }
    // }

    mesh.BuildBvh();
    model->GenerateRanges();
    model->FillBuffers();
    return model;
  }
}
