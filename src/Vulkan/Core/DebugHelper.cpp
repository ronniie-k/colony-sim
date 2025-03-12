#include "DebugHelper.h"

#include <unordered_map>

LogLevel DebugHelper::m_validationLogLevel = LogLevel::eTrace;
const std::vector<const char*> DebugHelper::m_validationLayers = { "VK_LAYER_KHRONOS_validation" };

namespace
{
std::unordered_map<VkDebugUtilsMessageSeverityFlagBitsEXT, LogLevel> severityMap = {
	{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, LogLevel::eTrace },
	{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, LogLevel::eInfo },
	{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, LogLevel::eWarn },
	{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, LogLevel::eErr },
};
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugHelper::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														  VkDebugUtilsMessageTypeFlagsEXT messageType,
														  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														  void* pUserData)
{
	auto severityLevel = severityMap[messageSeverity];
	if (m_validationLogLevel <= severityLevel)
	{
		if (severityLevel == LogLevel::eTrace)
			; // spdlog::trace("validation layer:\t{}", pCallbackData->pMessage);
		else if (severityLevel == LogLevel::eInfo)
			; // spdlog::info("validation layer:\t{}", pCallbackData->pMessage);
		else if (severityLevel == LogLevel::eWarn)
			; // spdlog::warn("validation layer:\t{}", pCallbackData->pMessage);
		else if (severityLevel == LogLevel::eErr)
			; // spdlog::error("validation layer:\t{}", pCallbackData->pMessage);
	}

	return VK_FALSE;
}

bool DebugHelper::validationLayersEnabled()
{
#ifdef DEBUG
	return true;
#else
	return false;
#endif
}

vk::DebugUtilsMessengerCreateInfoEXT DebugHelper::createDebugCreateInfo()
{
	vk::DebugUtilsMessengerCreateInfoEXT createInfo {};
	createInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
								  vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

	createInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
							  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

	createInfo.setPfnUserCallback(DebugHelper::debugCallback);
	return createInfo;
}

bool DebugHelper::validationLayersSupported()
{
	if (!validationLayersEnabled())
		return true;

	std::vector<vk::LayerProperties> layers = vk::enumerateInstanceLayerProperties();
	for (const char* validationLayerName : m_validationLayers)
	{
		for (const vk::LayerProperties& layer : layers)
		{
			if (strcmp(layer.layerName, validationLayerName) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

const std::vector<const char*>& DebugHelper::getValidationLayers()
{
	return m_validationLayers;
}

void DebugHelper::create(vk::Instance instance)
{
	m_instance = instance;
	auto createInfo = createDebugCreateInfo();
	auto createDebugUtilsMessengerFunc =
		reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));

	if (createDebugUtilsMessengerFunc == nullptr)
	{
		; // spdlog::critical("could not create debug messenger, aborting");
		throw std::runtime_error("could not create debug messenger");
	}

	VkResult result = createDebugUtilsMessengerFunc(static_cast<VkInstance>(instance),
													reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr,
													reinterpret_cast<VkDebugUtilsMessengerEXT*>(&m_debugMessenger));

	if (result != VK_SUCCESS)
	{
		; // spdlog::critical("could not create debug messenger, aborting");
		throw std::runtime_error("could not create debug messenger");
	}
}

void DebugHelper::destroy()
{
	auto destroyFunc = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(m_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

	if (destroyFunc != nullptr)
	{
		destroyFunc(static_cast<VkInstance>(m_instance), static_cast<VkDebugUtilsMessengerEXT>(m_debugMessenger), nullptr);
	}
}
