#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <fstream>
#include <optional>

#include <filesystem>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utils/Logging.hpp"

namespace vulkan_utils
{
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isValid() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapchainSupportInfo
{
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

inline void printAvailableExtensions()
{
	const std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

	Logging::Info("--available extensions--");
	for (const auto& extension : extensions)
	{
		Logging::Info("{}", extension.extensionName.data());
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
	std::filesystem::path cwd = std::filesystem::current_path();
	Logging::Info("attempting to open file from directory: {}", cwd.string());

	if (!file.is_open())
	{
		Logging::Error("failed to open file: {}", filename);
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