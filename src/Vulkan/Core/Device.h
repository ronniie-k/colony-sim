#pragma once

#include "vk_mem_alloc.h"
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

class VulkanInstance;

class VulkanDevice
{
public:
	~VulkanDevice();

	void create(vk::Instance instance, vk::SurfaceKHR surface);
	void destroy();

	vk::PhysicalDevice getPhysicalDevice() { return m_physicalDevice; }
	VmaAllocator getAllocator() { return m_allocator; }

	const vk::Queue& getGraphicsQueue() const { return m_graphicsQueue; }
	const vk::Queue& getPresentQueue() const { return m_presentQueue; }

	vk::Device handle;

private:
	void pickPhysicalDevice(vk::SurfaceKHR surface);
	void createDevice(vk::SurfaceKHR surface);
	void createAllocator();

	bool extensionsSupported(vk::PhysicalDevice device);
	bool isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface);

private:
	vk::Instance m_instance;
	vk::PhysicalDevice m_physicalDevice;
	VmaAllocator m_allocator;

	vk::Queue m_graphicsQueue;
	vk::Queue m_presentQueue;

	const std::vector<const char*> m_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};