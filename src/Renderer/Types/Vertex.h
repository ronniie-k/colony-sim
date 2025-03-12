#pragma once

#include <array>
#include <vulkan/vulkan.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;

	static vk::VertexInputBindingDescription getBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDesc;
		bindingDesc.setBinding(0);
		bindingDesc.setStride(sizeof(Vertex));
		bindingDesc.setInputRate(vk::VertexInputRate::eVertex);
		return bindingDesc;
	};

	static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescription()
	{
		std::array<vk::VertexInputAttributeDescription, 2> descriptions;
		descriptions[0].setBinding(0);
		descriptions[0].setLocation(0);
		descriptions[0].setFormat(vk::Format::eR32G32Sfloat);
		descriptions[0].setOffset(offsetof(Vertex, position));

		descriptions[1].setBinding(0);
		descriptions[1].setLocation(1);
		descriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
		descriptions[1].setOffset(offsetof(Vertex, color));
		return descriptions;
	}
};