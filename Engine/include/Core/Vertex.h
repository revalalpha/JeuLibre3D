#pragma once
#include "Global.h"
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec2 uv;
	static vk::VertexInputBindingDescription getBindingDescription() {
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}
	static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
						vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)),

			vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, color)),
			vk::VertexInputAttributeDescription(3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv))
		};

	}
	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && uv == other.uv;
	}
};

template <>
struct std::hash<Vertex>
{
	size_t operator()(Vertex const& vertex) const noexcept
	{
		return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
	}
};



struct SimpleVertex
{
	glm::vec3 pos;
	glm::vec4 color;

	static vk::VertexInputBindingDescription getBindingDescription() {
		return { 0, sizeof(SimpleVertex), vk::VertexInputRate::eVertex };
	}
	static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(SimpleVertex, pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(SimpleVertex, color))
		};
	}
	bool operator==(const SimpleVertex& other) const
	{
		return pos == other.pos && color == other.color ;
	}
};


template <>
struct std::hash<SimpleVertex>
{
	size_t operator()(SimpleVertex const& vertex) const noexcept
	{
		return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1);
	}
};