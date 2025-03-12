#pragma once

#include <vulkan/vulkan.hpp>

#include "Vulkan/Core/DebugHelper.h"

class Window;

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

	void create();
	void destroy();

	vk::Instance handle;

private:
	void createInstance();

private:
	DebugHelper m_debugHelper;
};