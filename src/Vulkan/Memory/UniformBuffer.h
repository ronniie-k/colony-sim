#pragma once

#include "Buffer.h"
// #include "Renderer/Types/UniformBufferData.h"

struct UniformBufferData
{
	int temp;
};

class VulkanUniformBuffer : public VulkanBuffer
{
public:
	void create(VulkanDevice& device)
	{
		vk::DeviceSize size = sizeof(UniformBufferData);
		VulkanBuffer::create(device, size, vk::BufferUsageFlagBits::eUniformBuffer);
	}
};