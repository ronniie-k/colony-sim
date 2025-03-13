#pragma once

#include "Buffer.h"

#include "Utils/Logging.hpp"

class VulkanIndexBuffer : public VulkanBuffer
{
public:
	void create(VulkanDevice& device, const std::vector<uint16_t>& indices)
	{
#ifdef DEBUG
		if (indices.size() >= 65534)
			Logging::Warning("16 bit index buffer limit reached");
#endif
		vk::DeviceSize size = sizeof(indices[0]) * indices.size();
		VulkanBuffer::create(device, size, vk::BufferUsageFlagBits::eIndexBuffer);
		copyData(indices.data(), size);
	}
};