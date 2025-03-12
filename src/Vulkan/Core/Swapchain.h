#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "Vulkan/Core/Device.h"

class GLFWwindow;

class VulkanSwapchain
{
public:
	void create(VulkanDevice& device, GLFWwindow* window);
	void createSurface(vk::Instance handle, GLFWwindow* window);
	void createFramebuffers(vk::RenderPass renderPass);
	void destroy();

	void recreate(GLFWwindow* window);

	vk::Framebuffer getFramebuffer(uint32_t index) const { return m_frameBuffers[index]; }
	vk::SurfaceKHR getSurface() { return m_surface; }
	vk::Format getFormat() { return m_format; }
	vk::Extent2D getExtent() { return m_extent; }

	vk::SwapchainKHR handle;

private:
	void createSwapchain(GLFWwindow* window);
	void createImageViews();

	vk::SurfaceFormatKHR chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> formats);
	vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
	vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

private:
	vk::Instance m_instance;
	vk::Device m_device;
	vk::PhysicalDevice m_physicalDevice;

	vk::SurfaceKHR m_surface;
	vk::RenderPass m_renderPass;
	std::vector<vk::Image> m_images;
	std::vector<vk::ImageView> m_imageViews;
	std::vector<vk::Framebuffer> m_frameBuffers;
	vk::Format m_format;
	vk::Extent2D m_extent;
};