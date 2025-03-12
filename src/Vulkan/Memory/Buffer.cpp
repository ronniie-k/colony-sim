#include "Buffer.h"
#include "Vulkan/Core/Device.h"
#include <vulkan/vulkan_core.h>

void VulkanBuffer::create(VulkanDevice& vulkanDevice, vk::DeviceSize size, vk::BufferUsageFlags usage)
{
	m_device = vulkanDevice.handle;
	m_physicalDevice = vulkanDevice.getPhysicalDevice();
	m_allocator = vulkanDevice.getAllocator();

	vk::BufferCreateInfo createInfo;
	createInfo.setUsage(usage);
	createInfo.setSharingMode(vk::SharingMode::eExclusive);
	createInfo.size = size;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	VkResult result = vmaCreateBuffer(vulkanDevice.getAllocator(), reinterpret_cast<VkBufferCreateInfo*>(&createInfo), &allocInfo,
									  reinterpret_cast<VkBuffer*>(&handle), &m_memory, nullptr);
}

void VulkanBuffer::copyData(const void* data, vk::DeviceSize size)
{
	void* mappedData;
	vmaMapMemory(m_allocator, m_memory, &mappedData);
	memcpy(mappedData, data, size);
	vmaUnmapMemory(m_allocator, m_memory);
}

void VulkanBuffer::destroy()
{
	if (handle != VK_NULL_HANDLE)
		vmaDestroyBuffer(m_allocator, static_cast<VkBuffer>(handle), m_memory);
}

VulkanBuffer::VulkanBuffer(VulkanBuffer&& other)
{
	destroy();

	handle = other.handle;
	m_device = other.m_device;
	m_physicalDevice = other.m_physicalDevice;
	m_allocator = other.m_allocator;
	m_memory = other.m_memory;

	other.handle = VK_NULL_HANDLE;
	other.m_device = VK_NULL_HANDLE;
	other.m_physicalDevice = VK_NULL_HANDLE;
	other.m_allocator = VK_NULL_HANDLE;
	other.m_memory = VK_NULL_HANDLE;
}

VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& rhs)
{
	if (this == &rhs)
		return *this;

	destroy();

	handle = rhs.handle;
	m_device = rhs.m_device;
	m_physicalDevice = rhs.m_physicalDevice;
	m_allocator = rhs.m_allocator;
	m_memory = rhs.m_memory;

	rhs.handle = VK_NULL_HANDLE;
	rhs.m_device = VK_NULL_HANDLE;
	rhs.m_physicalDevice = VK_NULL_HANDLE;
	rhs.m_allocator = VK_NULL_HANDLE;
	rhs.m_memory = VK_NULL_HANDLE;

	return *this;
}
