#include "GraphicsPipeline.h"
#include <cmath>
#include <vulkan/vulkan.hpp>

#include "Vulkan/Core/Utils.h"
#include "Renderer/Types/Vertex.h"

void VulkanGraphicsPipeline::create(vk::Device device,
									VulkanSwapchain& swapchain,
									const std::string& vertexSPV,
									const std::string& fragSPV,
									vk::DescriptorSetLayout layout)
{
	m_device = device;
	createRenderPass(swapchain.getFormat());
	createPipeline(vertexSPV, fragSPV, swapchain.getExtent(), layout);
}

void VulkanGraphicsPipeline::VulkanGraphicsPipeline::destroy()
{
	m_device.destroyPipeline(handle);
	m_device.destroyPipelineLayout(m_layout);
	m_device.destroyRenderPass(m_renderPass);
}

vk::PipelineShaderStageCreateInfo VulkanGraphicsPipeline::createShaderStage(const std::string& shaderSPV, vk::ShaderStageFlagBits stage)
{
	auto src = vulkan_utils::readFile(shaderSPV);
	vk::ShaderModuleCreateInfo shaderInfo;
	shaderInfo.codeSize = src.size();
	shaderInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

	vk::ShaderModule shader = m_device.createShaderModule(shaderInfo);
	m_cachedShaderModules.push_back(shader);

	vk::PipelineShaderStageCreateInfo createInfo;
	createInfo.setStage(stage);
	createInfo.setModule(shader);
	createInfo.setPName("main");

	return createInfo;
}

void VulkanGraphicsPipeline::VulkanGraphicsPipeline::createRenderPass(vk::Format swapchainFormat)
{
	vk::AttachmentDescription colorAttachment;
	colorAttachment.setFormat(swapchainFormat);
	colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
	colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
	colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
	colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
	colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colorAttachmentReference;
	colorAttachmentReference.setAttachment(0);
	colorAttachmentReference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpass;
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;

	vk::SubpassDependency dependency;
	dependency.setSrcSubpass(vk::SubpassExternal);
	dependency.setDstSubpass(0);
	dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	dependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
	dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo createInfo;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachment;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	m_renderPass = m_device.createRenderPass(createInfo);
}

void VulkanGraphicsPipeline::VulkanGraphicsPipeline::createPipeline(const std::string& vertexSPV,
																	const std::string& fragSPV,
																	vk::Extent2D swapchainExtent,
																	vk::DescriptorSetLayout layout)
{
	const std::string shaderRoot = vulkan_utils::getShaderRoot();
	std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {
		createShaderStage(shaderRoot + vertexSPV, vk::ShaderStageFlagBits::eVertex),
		createShaderStage(shaderRoot + fragSPV, vk::ShaderStageFlagBits::eFragment)
	};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescription();
	vk::PipelineVertexInputStateCreateInfo vertexInputState;
	vertexInputState.setVertexBindingDescriptionCount(1);
	vertexInputState.setPVertexBindingDescriptions(&bindingDescription);
	vertexInputState.setVertexAttributeDescriptions(attributeDescription);

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
	inputAssembly.setPrimitiveRestartEnable(false);

	// viewport/scissor
	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamicState;
	dynamicState.setDynamicStates(dynamicStates);

	vk::Viewport viewport;
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vk::Rect2D scissor;
	scissor.setOffset({ 0, 0 });
	scissor.setExtent(swapchainExtent);

	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// raserizer
	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.setDepthClampEnable(false);
	rasterizer.setRasterizerDiscardEnable(false);
	rasterizer.setPolygonMode(vk::PolygonMode::eFill);
	rasterizer.setLineWidth(1.f);
	rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
	rasterizer.setFrontFace(vk::FrontFace::eCounterClockwise);
	rasterizer.setDepthBiasEnable(false);
	rasterizer.setDepthBiasConstantFactor(0.f);
	rasterizer.setDepthBiasClamp(0.f);
	rasterizer.setDepthBiasSlopeFactor(0.f);

	vk::PipelineMultisampleStateCreateInfo multisampling;
	multisampling.setSampleShadingEnable(false);
	multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
	multisampling.setMinSampleShading(1.f);
	multisampling.setPSampleMask(nullptr);
	multisampling.setAlphaToCoverageEnable(false);
	multisampling.setAlphaToOneEnable(false);

	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR //clang-format off
		| vk::ColorComponentFlagBits::eG								 //clang-format off
		| vk::ColorComponentFlagBits::eB								 //clang-format off
		| vk::ColorComponentFlagBits::eA;								 //clang-format off
	colorBlendAttachment.blendEnable = false;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.setLogicOpEnable(false);
	colorBlending.setLogicOp(vk::LogicOp::eCopy);
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.f;
	colorBlending.blendConstants[1] = 0.f;
	colorBlending.blendConstants[2] = 0.f;
	colorBlending.blendConstants[3] = 0.f;

	vk::PipelineLayoutCreateInfo layoutCreateInfo;
	layoutCreateInfo.setLayoutCount = 1;
	layoutCreateInfo.pSetLayouts = &layout;

	m_layout = m_device.createPipelineLayout(layoutCreateInfo);

	vk::GraphicsPipelineCreateInfo createInfo;
	createInfo.setStages(shaderStages);
	createInfo.setPVertexInputState(&vertexInputState);
	createInfo.setPInputAssemblyState(&inputAssembly);
	createInfo.setPViewportState(&viewportState);
	createInfo.setPRasterizationState(&rasterizer);
	createInfo.setPMultisampleState(&multisampling);
	createInfo.setPDepthStencilState(nullptr);
	createInfo.setPColorBlendState(&colorBlending);
	createInfo.setPDynamicState(&dynamicState);
	createInfo.setLayout(m_layout);
	createInfo.setRenderPass(m_renderPass);
	createInfo.setSubpass(0);

	auto pipelines = m_device.createGraphicsPipelines(nullptr, createInfo);
	handle = pipelines.value[0];

	if (pipelines.result != vk::Result::eSuccess)
		; // spdlog::critical("failed to create pipeline");

	for (vk::ShaderModule shader : m_cachedShaderModules)
		m_device.destroyShaderModule(shader);

	m_cachedShaderModules.clear();
}
