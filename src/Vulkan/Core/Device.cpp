#include "Device.h"

#include <unordered_set>

#include "vk_mem_alloc.h"
#include "Vulkan/Core/DebugHelper.h"
#include "Vulkan/Core/Utils.h"

VulkanDevice::~VulkanDevice()
{
}

void VulkanDevice::create(vk::Instance instance, vk::SurfaceKHR surface)
{
	m_instance = instance;
	pickPhysicalDevice(surface);
	createDevice(surface);
	createAllocator();
}

void VulkanDevice::destroy()
{
	handle.destroy();
}

void VulkanDevice::createDevice(vk::SurfaceKHR surface)
{
	std::array<float, 1> priorities = { 1.f };
	vulkan_utils::QueueFamilyIndices indices = vulkan_utils::findQueueFamilies(m_physicalDevice, surface);
	std::unordered_set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		vk::DeviceQueueCreateInfo queueCreateInfo {};
		queueCreateInfo.setQueueFamilyIndex(queueFamily);
		queueCreateInfo.setQueueCount(1);
		queueCreateInfo.setQueuePriorities(priorities);
		queueCreateInfos.push_back(queueCreateInfo);
	}

	vk::PhysicalDeviceFeatures deviceFeatures;
	deviceFeatures.setSamplerAnisotropy(true);

	vk::DeviceCreateInfo createInfo;
	createInfo.setQueueCreateInfos(queueCreateInfos);
	createInfo.setPEnabledFeatures(&deviceFeatures);
	createInfo.setPEnabledExtensionNames(m_extensions);

	if (DebugHelper::validationLayersEnabled())
		createInfo.setPEnabledLayerNames(DebugHelper::getValidationLayers());
	else
		createInfo.enabledLayerCount = 0;

	handle = m_physicalDevice.createDevice(createInfo);

	m_graphicsQueue = handle.getQueue(indices.graphicsFamily.value(), 0);
	m_presentQueue = handle.getQueue(indices.presentFamily.value(), 0);
}


void VulkanDevice::VulkanDevice::createAllocator()
{
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo createInfo = {};
	createInfo.physicalDevice = m_physicalDevice;
	createInfo.device = handle;
	createInfo.instance = m_instance;
	createInfo.vulkanApiVersion = VK_API_VERSION_1_0;
	createInfo.pVulkanFunctions = &vulkanFunctions;
	vmaCreateAllocator(&createInfo, &m_allocator);
}

void VulkanDevice::pickPhysicalDevice(vk::SurfaceKHR surface)
{
	auto devices = m_instance.enumeratePhysicalDevices();

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device, surface))
		{
			m_physicalDevice = device;
			return;
		}
	}

	// spdlog::error("failed to find GPU");
	throw std::runtime_error("failed to find a suitable GPU");
}

bool VulkanDevice::extensionsSupported(vk::PhysicalDevice device)
{
	std::vector<vk::ExtensionProperties> extensions = device.enumerateDeviceExtensionProperties();
	std::unordered_set<std::string> requiredExtensions(m_extensions.begin(), m_extensions.end());

	for (const auto& extension : extensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool VulkanDevice::isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	vulkan_utils::QueueFamilyIndices indices = vulkan_utils::findQueueFamilies(device, surface);

	vk::PhysicalDeviceFeatures support = device.getFeatures();

	bool swapchainSupport = false;
	if (extensionsSupported(device))
	{
		auto swapchainInfo = vulkan_utils::getSwapchainSupportInfo(device, surface);
		swapchainSupport = !swapchainInfo.formats.empty() && !swapchainInfo.presentModes.empty();
	}

	return indices.isValid() && swapchainSupport && support.samplerAnisotropy;
}
