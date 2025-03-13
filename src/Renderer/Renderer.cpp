#include "Renderer.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <chrono>

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Vulkan/Core/Device.h"
#include "Vulkan/Memory/UniformBuffer.h"
#include "Vulkan/Pipeline/GraphicsPipeline.h"
#include "Vulkan/Core/Swapchain.h"
#include "Vulkan/Core/Utils.h"
#include "Types/Vertex.h"
#include "Utils/Logging.hpp"

namespace
{
const std::vector<Vertex> vertices = { { { -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } },
									   { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
									   { { 0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f } },
									   { { -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f } } };
const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
} // namespace

const uint32_t Renderer::m_framesInFlight = 2;

Renderer::Renderer()
{
}

void Renderer::initVulkan(Window* window)
{
	m_window = window;
	auto* glfwWindow = m_window->getGLFWWindow();

	m_instance.create();
	m_swapchain.createSurface(m_instance.handle, glfwWindow);
	m_device.create(m_instance.handle, m_swapchain.getSurface());
	m_swapchain.create(m_device, glfwWindow);

	createCommandObjects();
	createSyncObjects();

	/*for (int i = 0; i < m_framesInFlight; i++)
	{
		VulkanUniformBuffer buffer;
		buffer.create(m_device);
		m_uniformBuffers.emplace_back(std::move(buffer));
	}*/

	m_vertexBuffer.create(m_device, vertices);
	m_indexBuffer.create(m_device, indices);

	m_pipelineDescriptor.create(m_device.handle, m_framesInFlight, m_uniformBuffers);

	m_pipeline.create(m_device.handle, m_swapchain, "vert.spv", "frag.spv", m_pipelineDescriptor.getLayout());
	m_swapchain.createFramebuffers(m_pipeline.getRenderPass());

	// m_texture.create(m_device, "test.png");
	// transitionImageLayout(m_texture.handle, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined,
	// vk::ImageLayout::eTransferDstOptimal); copyBufferToImage(m_texture.getStagingBufferHandle(), m_texture.handle, m_texture.getWidth(),
	// m_texture.getHeight()); transitionImageLayout(m_texture.handle, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal,
	//					  vk::ImageLayout::eShaderReadOnlyOptimal);
	// m_texture.freeStagingBuffer();
	// m_sampler.create(m_device);
	Logging::Info("pass");
}

void Renderer::createCommandObjects()
{
	vulkan_utils::QueueFamilyIndices queueFamilies =
		vulkan_utils::findQueueFamilies(m_device.getPhysicalDevice(), m_swapchain.getSurface());

	vk::CommandPoolCreateInfo createInfo;
	createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	createInfo.setQueueFamilyIndex(queueFamilies.graphicsFamily.value());

	m_commandPool = m_device.handle.createCommandPool(createInfo);

	m_commandBuffers.resize(m_framesInFlight);

	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(m_commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(m_commandBuffers.size());

	m_commandBuffers = m_device.handle.allocateCommandBuffers(allocInfo);
}


void Renderer::createSyncObjects()
{
	m_imgAvailableSemaphores.resize(m_framesInFlight);
	m_renderFinishedSemaphores.resize(m_framesInFlight);
	m_inFlightFences.resize(m_framesInFlight);

	for (int i = 0; i < m_framesInFlight; i++)
	{
		vk::SemaphoreCreateInfo semaphoreCreateInfo;
		m_imgAvailableSemaphores[i] = m_device.handle.createSemaphore(semaphoreCreateInfo);
		m_renderFinishedSemaphores[i] = m_device.handle.createSemaphore(semaphoreCreateInfo);

		vk::FenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
		m_inFlightFences[i] = m_device.handle.createFence(fenceCreateInfo);
	}
}

vk::CommandBuffer Renderer::beginSingleTimeCommands()
{
	vk::CommandBuffer cmd;
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandPool(m_commandPool);
	allocInfo.setCommandBufferCount(1);

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	auto result = m_device.handle.allocateCommandBuffers(&allocInfo, &cmd);
	cmd.begin(&beginInfo);

	return cmd;
}

void Renderer::endSingleTimeCommands(vk::CommandBuffer cmd)
{
	cmd.end();

	vk::SubmitInfo submitInfo;
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&cmd);

	vk::Queue graphicsQueue = m_device.getGraphicsQueue();
	graphicsQueue.submit(1, &submitInfo, nullptr);
	graphicsQueue.waitIdle();

	m_device.handle.freeCommandBuffers(m_commandPool, 1, &cmd);
}


void Renderer::recordCommandBuffer(vk::CommandBuffer cmdBuffer, uint32_t imgIndex)
{
	vk::CommandBufferBeginInfo info;
	cmdBuffer.begin(info);

	vk::ClearValue clearColor = vk::ClearColorValue { 0.f, 0.f, 0.f, 1.f };

	vk::RenderPassBeginInfo renderPassInfo;
	renderPassInfo.setRenderPass(m_pipeline.getRenderPass());
	renderPassInfo.setFramebuffer(m_swapchain.getFramebuffer(imgIndex));
	renderPassInfo.renderArea.offset = vk::Offset2D { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapchain.getExtent();
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	auto extent = m_swapchain.getExtent();
	vk::Viewport viewport;
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vk::Rect2D scissor {};
	scissor.offset = vk::Offset2D { 0, 0 };
	scissor.extent = extent;

	std::array<vk::Buffer, 1> vertexBuffers = { m_vertexBuffer.handle };
	std::array<vk::DeviceSize, 1> offsets = { 0 };

	cmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.handle);
	cmdBuffer.setViewport(0, 1, &viewport);
	cmdBuffer.setScissor(0, 1, &scissor);
	cmdBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
	cmdBuffer.bindIndexBuffer(m_indexBuffer.handle, 0, vk::IndexType::eUint16);
	// cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline.getLayout(), 0, 1,
	//								 &m_pipelineDescriptor.getDescriptorSet(m_currentFrame), 0, nullptr);
	cmdBuffer.drawIndexed(indices.size(), 1, 0, 0, 0);
	cmdBuffer.draw(3, 1, 0, 0);

	cmdBuffer.endRenderPass();
	cmdBuffer.end();
}

void Renderer::drawFrame()
{
	VulkanSwapchain& swapchain = m_swapchain;

	(void) m_device.handle.waitForFences(1, &m_inFlightFences[m_currentFrame], true, UINT64_MAX);

	auto nextImgResult = m_device.handle.acquireNextImageKHR(swapchain.handle, UINT64_MAX, m_imgAvailableSemaphores[m_currentFrame]);
	uint32_t imgIndex = nextImgResult.value;

	if (nextImgResult.result == vk::Result::eErrorOutOfDateKHR)
	{
		swapchain.recreate(m_window->getGLFWWindow());
		return;
	}
	else if (nextImgResult.result != vk::Result::eSuccess && nextImgResult.result != vk::Result::eSuboptimalKHR)
	{
		throw std::runtime_error("failed to acquire swapchain img");
	}

	(void) m_device.handle.resetFences(1, &m_inFlightFences[m_currentFrame]);

	updateUniformBuffer();

	m_commandBuffers[m_currentFrame].reset();
	recordCommandBuffer(m_commandBuffers[m_currentFrame], imgIndex);

	std::array<vk::Semaphore, 1> waitSemaphores = { m_imgAvailableSemaphores[m_currentFrame] };
	std::array<vk::Semaphore, 1> signalSemaphores = { m_renderFinishedSemaphores[m_currentFrame] };
	std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	std::array<vk::CommandBuffer, 1> cmdBuffers = { m_commandBuffers[m_currentFrame] };
	vk::SubmitInfo submitInfo;
	submitInfo.setWaitSemaphores(waitSemaphores);
	submitInfo.setSignalSemaphores(signalSemaphores);
	submitInfo.setWaitDstStageMask(waitStages);
	submitInfo.setCommandBuffers(cmdBuffers);

	(void) m_device.getGraphicsQueue().submit(1, &submitInfo, m_inFlightFences[m_currentFrame]);

	vk::PresentInfoKHR presentInfo;
	presentInfo.setWaitSemaphores(signalSemaphores);
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.handle;
	presentInfo.pImageIndices = &imgIndex;

	vk::Result result = m_device.getPresentQueue().presentKHR(presentInfo);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_window->hasResized())
	{
		m_window->setResized(false);
		swapchain.recreate(m_window->getGLFWWindow());
	}
	else if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to present swapchain img");
	}

	m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;
}

void Renderer::updateUniformBuffer()
{
	/*	static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		auto swapchianExtent = m_swapchain.getExtent();

		UniformBufferData ubo {};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		ubo.proj = glm::perspective(glm::radians(45.0f), swapchianExtent.width / (float) swapchianExtent.height, 0.1f, 10.0f);

		ubo.proj[1][1] *= -1;

		m_uniformBuffers[m_currentFrame].copyData(&ubo, sizeof(UniformBufferData));*/
}

void Renderer::transitionImageLayout(vk::Image img, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::CommandBuffer cmd = beginSingleTimeCommands();

	vk::ImageSubresourceRange subresourceRange;
	subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	subresourceRange.setBaseMipLevel(0);
	subresourceRange.setLevelCount(1);
	subresourceRange.setBaseArrayLayer(0);
	subresourceRange.setLayerCount(1);

	vk::ImageMemoryBarrier barrier;
	barrier.setOldLayout(oldLayout);
	barrier.setNewLayout(newLayout);
	barrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
	barrier.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
	barrier.setImage(img);
	barrier.setSubresourceRange(subresourceRange);

	vk::PipelineStageFlags srcStage;
	vk::PipelineStageFlags dstStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
		barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
		srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
		barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
		srcStage = vk::PipelineStageFlagBits::eTransfer;
		dstStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		Logging::Warning("unsupported layout transition");
	}

	cmd.pipelineBarrier(srcStage, dstStage, vk::DependencyFlagBits(0), 0, nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(cmd);
}

void Renderer::waitIdle()
{
	m_device.handle.waitIdle();
}

void Renderer::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	auto cmd = beginSingleTimeCommands();

	vk::ImageSubresourceLayers subLayers;
	subLayers.setAspectMask(vk::ImageAspectFlagBits::eColor);
	subLayers.setMipLevel(0);
	subLayers.setBaseArrayLayer(0);
	subLayers.setLayerCount(1);

	vk::BufferImageCopy region;
	region.setBufferOffset(0);
	region.setBufferRowLength(0);
	region.setBufferImageHeight(0);
	region.setImageOffset(vk::Offset3D(0, 0, 0));
	region.setImageExtent(vk::Extent3D(width, height, 1));
	region.setImageSubresource(subLayers);

	cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	endSingleTimeCommands(cmd);
}
