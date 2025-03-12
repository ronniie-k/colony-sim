#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <fstream>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vulkan_utils
{
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isValid() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapchainSupportInfo
{
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

inline void printAvailableExtensions()
{
	std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

	// spdlog::info("--available extensions--");
	for (const auto& extension : extensions)
	{
		; // spdlog::info("{}", extension.extensionName.data());
	}
}

inline std::vector<const char*> getRequiredExtensions(bool validationLayersEnabled)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (validationLayersEnabled)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

inline std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		// spdlog::critical("failed to open file: {}", filename);
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

inline const std::string getShaderRoot()
{
	// fix this when i decide how i want to store my shader files
	return "res/shaders/output/";
}

inline const std::string getTextureRoot()
{
	// fix this, same as above
	return "res/textures/";
}

inline QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;
	auto queueFamilyProperties = device.getQueueFamilyProperties();

	for (int i = 0; i < queueFamilyProperties.size(); i++)
	{
		const auto& queueFamily = queueFamilyProperties[i];

		if (device.getSurfaceSupportKHR(i, surface))
		{
			indices.presentFamily = i;
		}

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;
		}

		if (indices.isValid())
			break;
	}

	return indices;
}

inline SwapchainSupportInfo getSwapchainSupportInfo(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	SwapchainSupportInfo info;
	info.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	info.formats = device.getSurfaceFormatsKHR(surface);
	info.presentModes = device.getSurfacePresentModesKHR(surface);

	return info;
}

} // namespace vulkan_utils