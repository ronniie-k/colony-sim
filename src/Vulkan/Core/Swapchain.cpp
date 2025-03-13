#include "Swapchain.h"
#include "Vulkan/Core/Utils.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void VulkanSwapchain::create(VulkanDevice& device, GLFWwindow* window)
{
	m_device = device.handle;
	m_physicalDevice = device.getPhysicalDevice();

	createSwapchain(window);
	createImageViews();
}

void VulkanSwapchain::VulkanSwapchain::destroy()
{
	for (auto framebuffer : m_frameBuffers)
		m_device.destroyFramebuffer(framebuffer);
	m_frameBuffers.clear();
	m_imageViews.clear();
	m_device.destroySwapchainKHR(handle);
}

void VulkanSwapchain::createSurface(vk::Instance instance, GLFWwindow* window)
{
	m_instance = instance;
	if (glfwCreateWindowSurface(m_instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_surface)) != VK_SUCCESS)
		throw std::runtime_error("failed to create vulkan surface!");
}

void VulkanSwapchain::createSwapchain(GLFWwindow* window)
{
	const vulkan_utils::SwapchainSupportInfo info = vulkan_utils::getSwapchainSupportInfo(m_physicalDevice, m_surface);

	const vk::SurfaceFormatKHR format = chooseSurfaceFormat(info.formats);
	const vk::PresentModeKHR presentMode = choosePresentMode(info.presentModes);
	const vk::Extent2D extent = chooseExtent(info.capabilities, window);

	uint32_t imgCount = info.capabilities.minImageCount + 1;

	if (info.capabilities.maxImageCount > 0 && imgCount > info.capabilities.maxImageCount)
		imgCount = info.capabilities.maxImageCount;

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.setSurface(m_surface);
	createInfo.setMinImageCount(imgCount);
	createInfo.setImageFormat(format.format);
	createInfo.setImageColorSpace(format.colorSpace);
	createInfo.setImageExtent(extent);
	createInfo.setImageArrayLayers(1);
	createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

	const vulkan_utils::QueueFamilyIndices indices = vulkan_utils::findQueueFamilies(m_physicalDevice, m_surface);
	const std::array<uint32_t, 2> queueFamilyIndices = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		createInfo.setQueueFamilyIndices(queueFamilyIndices);
	}
	else
	{
		createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	}

	createInfo.setPreTransform(info.capabilities.currentTransform);
	createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	createInfo.setPresentMode(presentMode);
	createInfo.setClipped(true);
	createInfo.setOldSwapchain(VK_NULL_HANDLE);

	handle = m_device.createSwapchainKHR(createInfo);

	m_images = m_device.getSwapchainImagesKHR(handle);
	m_format = format.format;
	m_extent = extent;
}

void VulkanSwapchain::createImageViews()
{
	m_imageViews.resize(m_images.size());
	for (size_t i = 0; i < m_images.size(); i++)
	{
		vk::ImageViewCreateInfo createInfo;

		createInfo.setImage(m_images[i]);
		createInfo.setViewType(vk::ImageViewType::e2D);
		createInfo.setFormat(m_format);
		createInfo.components.r = vk::ComponentSwizzle::eIdentity;
		createInfo.components.g = vk::ComponentSwizzle::eIdentity;
		createInfo.components.b = vk::ComponentSwizzle::eIdentity;
		createInfo.components.a = vk::ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		m_imageViews[i] = m_device.createImageView(createInfo);
	}
}

void VulkanSwapchain::createFramebuffers(vk::RenderPass renderPass)
{
	m_renderPass = renderPass;
	for (int i = 0; i < m_images.size(); i++)
	{
		std::array<vk::ImageView, 1> attachments = { m_imageViews[i] };
		vk::FramebufferCreateInfo createInfo;
		createInfo.setRenderPass(renderPass);
		createInfo.setAttachments(attachments);
		createInfo.setWidth(m_extent.width);
		createInfo.setHeight(m_extent.height);
		createInfo.setLayers(1);

		auto fb = m_device.createFramebuffer(createInfo);
		m_frameBuffers.emplace_back(fb);
	}
}

vk::SurfaceFormatKHR VulkanSwapchain::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const
{
	for (const auto& format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return format;
	}
	return formats[0];
}

vk::PresentModeKHR VulkanSwapchain::choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes) const
{
	for (const auto& mode : presentModes)
	{
		if (mode == vk::PresentModeKHR::eMailbox)
			return mode;
	}
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapchain::chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		vk::Extent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VulkanSwapchain::VulkanSwapchain::VulkanSwapchain::recreate(GLFWwindow* window)
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	m_device.waitIdle();

	destroy();
	createSwapchain(window);
	createImageViews();
	createFramebuffers(m_renderPass);
}
