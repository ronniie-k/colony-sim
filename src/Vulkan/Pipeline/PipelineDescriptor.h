#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include "Vulkan/Memory/UniformBuffer.h"

using DebugDescriptorInfo = std::tuple<vk::DescriptorType, uint32_t, vk::ShaderStageFlags>;

class PipelineDescriptor
{
public:
	void create(vk::Device device, uint32_t framesInFlight, const std::vector<VulkanUniformBuffer>& ubos);
	void destroy();

	void addResource(vk::DescriptorType type, uint32_t count, vk::ShaderStageFlags stage);

	vk::DescriptorSetLayout getLayout() { return m_layout; }
	vk::DescriptorSet& getDescriptorSet(uint32_t index) { return m_descriptorSets[index]; }
	void printDebugInfo();

private:
	void createDescriptorSetLayout();
	void createDescriptorPool(uint32_t framesInFlight);
	void createDescriptorSets(uint32_t framesInFlight, const std::vector<VulkanUniformBuffer>& ubos);

private:
	vk::DescriptorSetLayout m_layout;
	vk::DescriptorPool m_descriptorPool;
	std::vector<vk::DescriptorSet> m_descriptorSets;
	vk::Device m_device;

	uint32_t m_currentBinding = 0;
	uint32_t m_maxSets = 0;
	std::vector<vk::DescriptorSetLayoutBinding> m_layoutBindings;
	std::vector<vk::DescriptorPoolSize> m_poolSizes;
	std::vector<DebugDescriptorInfo> m_debugInfo;
};