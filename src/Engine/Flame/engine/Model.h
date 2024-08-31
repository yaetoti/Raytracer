#pragma once
#include <functional>
#include <vector>
#include <assimp/scene.h>

#include "Mesh.h"
#include "Flame/graphics/Vertex.h"
#include "Flame/graphics/buffers/IndexBuffer.h"
#include "Flame/graphics/buffers/VertexBuffer.h"

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

		bool Hit(const Ray& r, HitRecord<const Model*>& record, float tMin, float tMax) const;

		void Reset();
		void Parse(const aiScene& scene);

		void GenerateRanges();
		void FillBuffers();

  public:
		std::vector<Mesh> m_meshes;
		std::vector<MeshRange> m_ranges;
		VertexBuffer<Vertex> m_vertices;
		IndexBuffer m_indices;
		uint32_t m_vertexNum = 0;
		uint32_t m_indexNum = 0;
  };
}
