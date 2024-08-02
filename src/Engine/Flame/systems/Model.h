#pragma once
#include <functional>
#include <vector>
#include <assimp/scene.h>

#include "IndexBuffer.h"
#include "Mesh.h"
#include "Vertex.h"
#include "VertexBuffer.h"

namespace Flame {
  struct Model final {
    struct MeshRange final {
			MeshRange() = default;

			uint32_t vertexOffset;
			uint32_t indexOffset;
			uint32_t vertexNum;
			uint32_t indexNum;
		};

		Model() = default;

		bool Hit(const Ray& r, HitRecord& record, float tMin, float tMax) const {
		  if (m_meshes.empty()) {
		    return false;
		  }

			bool wasHit = false;
			for (const auto& mesh : m_meshes) {
			  if (mesh.Hit(r, record, tMin, tMax)) {
					wasHit = true;
			    tMax = record.time;
			  }
			}

			if (wasHit) {
				record.hitable = const_cast<Model*>(this);
			}

			return wasHit;
		}

		void Reset() {
		  m_meshes.clear();
			m_ranges.clear();
			m_vertices.Reset();
			m_indices.Reset();
			m_vertexNum = 0;
			m_indexNum = 0;
		}

		void Parse(const aiScene& scene) {
			Reset();
			m_meshes.reserve(scene.mNumMeshes);

			auto node = scene.mRootNode;
			std::function<void(const aiNode&)> ParseNode;

			for (uint32_t meshId = 0; meshId < scene.mNumMeshes; ++meshId) {
			  auto& mesh = m_meshes.emplace_back();
				mesh.Parse(*scene.mMeshes[meshId]);
			}

			//ParseNode = [this, &scene, &ParseNode](const aiNode& node) {
			//	assert(sizeof(glm::mat4) == sizeof(aiMatrix4x4));

			//	// TODO incorrect. Better to accumulate transforms in a stack
			//	// TODO Also take into account that there are empty compound nodes with transforms
			//	glm::mat4 nodeTransform { *reinterpret_cast<const glm::mat4*>(&node.mTransformation) };
			//	glm::mat4 nodeTransformInv { glm::inverse(nodeTransform) };

			//	// Parse meshes in this node
			//	for (uint32_t i = 0; i < node.mNumMeshes; ++i) {
			//		uint32_t meshId = node.mMeshes[i];
			//		auto& mesh = m_meshes.emplace_back();
			//		mesh.Parse(*scene.mMeshes[meshId]);
			//		mesh.transforms.push_back(nodeTransform);
			//		mesh.transformsInv.push_back(nodeTransformInv);
			//	}

			//	// Parse children nodes
			//  for (uint32_t i = 0; i < node.mNumChildren; ++i) {
			//    ParseNode(*node.mChildren[i]);
			//  }
			//};

			//ParseNode(*node);
			GenerateRanges();
			FillBuffers();
		}

  private:
		void GenerateRanges() {
		  uint32_t vertexOffset = 0;
		  uint32_t indexOffset = 0;
			m_ranges.reserve(m_meshes.size());

			for (uint32_t i = 0; i < m_meshes.size(); ++i) {
			  const auto& mesh = m_meshes[i];
				auto& range = m_ranges.emplace_back();

				range.vertexOffset = vertexOffset;
				range.indexOffset = indexOffset;
				range.vertexNum = static_cast<uint32_t>(mesh.vertices.size());
				range.indexNum = static_cast<uint32_t>(mesh.faces.size()) * 3U;

			  m_vertexNum += range.vertexNum;
			  m_indexNum += range.indexNum;

				vertexOffset += range.vertexNum;
				indexOffset += range.indexNum;
			}
		}

		void FillBuffers() {

			// Load vertices
			{
				std::vector<Vertex> vertexData;
				std::vector<uint32_t> indexData;
				vertexData.reserve(m_vertexNum);
				indexData.resize(m_indexNum);

				uint32_t indexOffset = 0;
			  for (uint32_t rangeId = 0; rangeId < m_ranges.size(); ++rangeId) {
					const auto& range = m_ranges[rangeId];
					const auto& mesh = m_meshes[rangeId];

					for (uint32_t verticeId = 0; verticeId < range.vertexNum; ++verticeId) {
						vertexData.emplace_back(mesh.vertices[verticeId], mesh.normals[verticeId]);
					}

					assert(reinterpret_cast<const char*>(indexData.data() + indexOffset) + range.indexNum * sizeof(uint32_t) <= reinterpret_cast<const char*>(indexData.data() + indexData.size()));
					//std::memcpy(indexData.data() + indexOffset, mesh.faces.data(), range.indexNum * sizeof(uint32_t));

			    for (uint32_t i = 0; i < range.indexNum / 3; ++i) {
					  indexData[indexOffset + i * 3] = mesh.faces[i].indices[0] + indexOffset;
					  indexData[indexOffset + i * 3 + 1] = mesh.faces[i].indices[1] + indexOffset;
					  indexData[indexOffset + i * 3 + 2] = mesh.faces[i].indices[2] + indexOffset;
					}

			    indexOffset += range.indexNum;
			  }

				HRESULT result;
				result = m_vertices.Init(vertexData.data(), m_vertexNum);
				assert(SUCCEEDED(result));
				result = m_indices.Init(indexData.data(), m_indexNum);
				assert(SUCCEEDED(result));
			}
		}

  public:
		std::vector<Mesh> m_meshes;
		std::vector<MeshRange> m_ranges;
		VertexBuffer<Vertex> m_vertices;
		IndexBuffer m_indices;
		uint32_t m_vertexNum = 0;
		uint32_t m_indexNum = 0;
  };
}
