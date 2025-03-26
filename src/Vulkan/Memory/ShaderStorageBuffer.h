#pragma once

#include "Buffer.h"
#include "Renderer/Types/QuadShaderData.h"


class VulkanStoageBuffer : public VulkanBuffer
{
public:
	void create(VulkanDevice& device, vk::DeviceSize size) { VulkanBuffer::create(device, size, vk::BufferUsageFlagBits::eStorageBuffer); }
};