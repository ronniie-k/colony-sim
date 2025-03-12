#pragma once

#include "Vulkan/Core/Device.h"
#include "vk_mem_alloc.h"
#include <vulkan/vulkan_enums.hpp>


class VulkanBuffer
{
public:
	VulkanBuffer() = default;
	~VulkanBuffer() = default;

	VulkanBuffer(const VulkanBuffer&) = delete;
	VulkanBuffer& operator=(const VulkanBuffer&) = delete;

	VulkanBuffer(VulkanBuffer&& other);
	VulkanBuffer& operator=(VulkanBuffer&& rhs);

	void create(VulkanDevice& vulkanDevice, vk::DeviceSize size, vk::BufferUsageFlags usage);
	void copyData(const void* data, vk::DeviceSize size);
	void destroy();

	vk::Buffer handle;

protected:
	vk::Device m_device;
	vk::PhysicalDevice m_physicalDevice;
	VmaAllocator m_allocator;
	VmaAllocation m_memory;
};