#pragma once

#include "Vulkan/Core/Swapchain.h"
#include <vulkan/vulkan.hpp>
#include <vector>

class VulkanDevice;

class VulkanGraphicsPipeline
{
public:
	void create(vk::Device device,
				VulkanSwapchain& swapchain,
				const std::string& vertexSPV,
				const std::string& fragSPV,
				vk::DescriptorSetLayout layout);
	void destroy();

	vk::RenderPass getRenderPass() { return m_renderPass; }
	vk::PipelineLayout getLayout() { return m_layout; }

	vk::Pipeline handle;

private:
	vk::PipelineShaderStageCreateInfo createShaderStage(const std::string& shaderSPV, vk::ShaderStageFlagBits stage);
	void createRenderPass(vk::Format swapchainFormat);
	void createPipeline(const std::string& vertexSPV,
						const std::string& fragSPV,
						vk::Extent2D swapchainExtent,
						vk::DescriptorSetLayout layout);

private:
	vk::Device m_device;

	vk::PipelineLayout m_layout;
	vk::RenderPass m_renderPass;
	std::vector<vk::ShaderModule> m_cachedShaderModules;
};