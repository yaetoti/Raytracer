#include "Model.h"
#include "glm/ext.hpp"

namespace Flame {
	bool Model::Hit(const Ray& r, HitRecord<const Model*>& record, float tMin, float tMax) const {
	  if (m_meshes.empty()) {
	    return false;
	  }
	
		HitRecord<const Mesh*> record0;
		for (const auto& mesh : m_meshes) {
		  if (mesh.Hit(r, record0, tMin, tMax)) {
		    tMax = record0.time;
		  }
		}
	
		if (record0.data != nullptr) {
			record = record0;
			record.data = this;
			return true;
		}
	
		return false;
	}

	void Model::Reset() {
	  m_meshes.clear();
		m_ranges.clear();
		m_vertices.Reset();
		m_indices.Reset();
		m_vertexNum = 0;
		m_indexNum = 0;
	}

	void Model::Parse(const aiScene& scene) {
		Reset();
		m_meshes.reserve(scene.mNumMeshes);
	
		auto node = scene.mRootNode;
	
		for (uint32_t meshId = 0; meshId < scene.mNumMeshes; ++meshId) {
		  auto& mesh = m_meshes.emplace_back();
			mesh.Parse(*scene.mMeshes[meshId]);
			// TODO Parse textures
		}
	
		std::function<void(aiNode*)> LoadInstances;
		LoadInstances = [&LoadInstances, this](aiNode* node) {
			const glm::mat4 nodeToParent = reinterpret_cast<const glm::mat4&>(node->mTransformation.Transpose());
			const glm::mat4 parentToNode = glm::inverse(nodeToParent);

			// The same node may contain multiple meshes in its space, referring to them by indices
			for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
				uint32_t meshIndex = node->mMeshes[i];
				m_meshes[meshIndex].transforms.push_back(nodeToParent); 
				m_meshes[meshIndex].transformsInv.push_back(parentToNode);
			}

			for (uint32_t i = 0; i < node->mNumChildren; ++i) {
				LoadInstances(node->mChildren[i]);
			}
		};
	
		LoadInstances(node);
		GenerateRanges();
		FillBuffers();
	}

	void Model::GenerateRanges() {
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

	void Model::FillBuffers() {
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
					vertexData.emplace_back(
						mesh.vertices[verticeId],
						mesh.normals[verticeId],
						mesh.tangents[verticeId],
						mesh.bitangents[verticeId],
						mesh.uvs[verticeId]
					);
				}
	
				assert(reinterpret_cast<const char*>(indexData.data() + indexOffset) + range.indexNum * sizeof(uint32_t) <= reinterpret_cast<const char*>(indexData.data() + indexData.size()));
				//std::memcpy(indexData.data() + indexOffset, mesh.faces.data(), range.indexNum * sizeof(uint32_t));
	
		    // for (uint32_t i = 0; i < range.indexNum / 3; ++i) {
				//   indexData[indexOffset + i * 3] = mesh.faces[i].indices[0] + indexOffset;
				//   indexData[indexOffset + i * 3 + 1] = mesh.faces[i].indices[1] + indexOffset;
				//   indexData[indexOffset + i * 3 + 2] = mesh.faces[i].indices[2] + indexOffset;
				// }

		    for (uint32_t i = 0; i < range.indexNum / 3; ++i) {
				  indexData[indexOffset + i * 3] = mesh.faces[i].indices[0];
				  indexData[indexOffset + i * 3 + 1] = mesh.faces[i].indices[1];
				  indexData[indexOffset + i * 3 + 2] = mesh.faces[i].indices[2];
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
}
