#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <vector>

namespace Havoc::Graphics {
	enum class VertexAttributeType {
		Float2,
		Float3,
		Float4
	};

	struct VertexAttribute {
		uint32_t location;
		VertexAttributeType type;
		size_t offset;
	};

	struct VertexLayout {
		std::vector<VertexAttribute> attributes;
		uint32_t stride;
	};

	namespace Simple {
		struct Vertex {
			glm::vec3 position;
			glm::vec4 color;

			static VertexLayout getLayout() {
				VertexLayout layout;
				layout.attributes = {
					{ 0, VertexAttributeType::Float3, offsetof(Vertex, position) },
					{ 1, VertexAttributeType::Float4, offsetof(Vertex, color) }
				};
				layout.stride = sizeof(Vertex);
				return layout;
			}
		};
	}
}