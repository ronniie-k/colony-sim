#pragma once

#include <cstdint>

#include "Vulkan/Core/Instance.h"
#include "Vulkan/Core/Window.h"
// #include "Vulkan/Core/image/Sampler.h"
// #include "Vulkan/Core/image/Texture.h"
#include "Vulkan/Memory/IndexBuffer.h"
#include "Vulkan/Memory/UniformBuffer.h"
#include "Vulkan/Pipeline/GraphicsPipeline.h"
#include "Vulkan/Memory/VertexBuffer.h"
#include "Vulkan/Pipeline/PipelineDescriptor.h"
// #include "Vulkan/Core/image/Texture.h"

class Renderer
{
public:
	Renderer();

	void initVulkan(Window* window);
	void cleanup();

	vk::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::CommandBuffer cmd);

	void recordCommandBuffer(vk::CommandBuffer cmdBuffer, uint32_t imgIndex);

	void transitionImageLayout(vk::Image img, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

	void drawFrame();
	void waitIdle();

	static const uint32_t getFramesInFlight() { return m_framesInFlight; }

private:
	void createCommandObjects();
	void createSyncObjects();

	void updateUniformBuffer();

private:
	VulkanInstance m_instance;
	VulkanDevice m_device;
	VulkanSwapchain m_swapchain;

	PipelineDescriptor m_pipelineDescriptor;
	VulkanGraphicsPipeline m_pipeline;
	Window* m_window = nullptr;

	static const uint32_t m_framesInFlight;
	uint32_t m_currentFrame = 0;

	vk::CommandPool m_commandPool;
	std::vector<vk::CommandBuffer> m_commandBuffers;

	std::vector<vk::Semaphore> m_imgAvailableSemaphores;
	std::vector<vk::Semaphore> m_renderFinishedSemaphores;
	std::vector<vk::Fence> m_inFlightFences;

	VulkanVertexBuffer m_vertexBuffer;
	VulkanIndexBuffer m_indexBuffer;
	std::vector<VulkanUniformBuffer> m_uniformBuffers;

	//VulkanTexture m_texture;
	//VulkanSampler m_sampler;
};