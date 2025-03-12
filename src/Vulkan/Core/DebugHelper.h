#pragma once
#include <vulkan/vulkan.hpp>

enum class LogLevel : int
{
	eTrace,
	eInfo,
	eWarn,
	eErr,
};

class DebugHelper
{
public:
	static LogLevel m_validationLogLevel;
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);
	static bool validationLayersEnabled();
	static vk::DebugUtilsMessengerCreateInfoEXT createDebugCreateInfo();
	static bool validationLayersSupported();
	static const std::vector<const char*>& getValidationLayers();

	void create(vk::Instance instance);
	void destroy();

private:
	vk::Instance m_instance;
	vk::DebugUtilsMessengerEXT m_debugMessenger;

	static const std::vector<const char*> m_validationLayers;
};