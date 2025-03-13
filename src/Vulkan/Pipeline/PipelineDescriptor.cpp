#include "PipelineDescriptor.h"
#include <cstdint>

#include <sys/types.h>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>

#include "Renderer/Renderer.h"

void PipelineDescriptor::create(vk::Device device, uint32_t framesInFlight, const std::vector<VulkanUniformBuffer>& ubos)
{
	m_device = device;
	createDescriptorSetLayout();
	createDescriptorPool(framesInFlight);
	createDescriptorSets(framesInFlight, ubos);
	return;
	vk::DescriptorSetLayoutCreateInfo layoutCreateInfo;
	layoutCreateInfo.setBindings(m_layoutBindings);

	m_layout = m_device.createDescriptorSetLayout(layoutCreateInfo);

	vk::DescriptorPoolCreateInfo poolCreateInfo;
	poolCreateInfo.setPoolSizes(m_poolSizes);
	poolCreateInfo.setMaxSets(m_maxSets);

	m_descriptorPool = m_device.createDescriptorPool(poolCreateInfo);
}

void PipelineDescriptor::destroy()
{
	m_device.destroyDescriptorPool(m_descriptorPool);
	m_device.destroyDescriptorSetLayout(m_layout);
}

void PipelineDescriptor::createDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding layoutBinding;
	layoutBinding.setBinding(0);
	layoutBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
	layoutBinding.setDescriptorCount(1);
	layoutBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	vk::DescriptorSetLayoutCreateInfo createInfo;
	createInfo.setBindingCount(1);
	createInfo.setPBindings(&layoutBinding);

	m_layout = m_device.createDescriptorSetLayout(createInfo);
}

void PipelineDescriptor::createDescriptorPool(uint32_t framesInFlight)
{
	vk::DescriptorPoolSize poolSize;
	poolSize.setType(vk::DescriptorType::eUniformBuffer);
	poolSize.setDescriptorCount(framesInFlight);

	vk::DescriptorPoolCreateInfo createInfo;
	createInfo.setPoolSizeCount(1);
	createInfo.setPPoolSizes(&poolSize);
	createInfo.setMaxSets(framesInFlight);

	m_descriptorPool = m_device.createDescriptorPool(createInfo);
}

void PipelineDescriptor::createDescriptorSets(uint32_t framesInFlight, const std::vector<VulkanUniformBuffer>& ubos)
{
	std::vector<vk::DescriptorSetLayout> layouts(framesInFlight, m_layout);

	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.setDescriptorPool(m_descriptorPool);
	allocInfo.setDescriptorSetCount(framesInFlight);
	allocInfo.setSetLayouts(layouts);

	m_descriptorSets = m_device.allocateDescriptorSets(allocInfo);
	return;
	for (int i = 0; i < framesInFlight; i++)
	{
		vk::DescriptorBufferInfo bufferInfo;
		bufferInfo.setBuffer(ubos[i].handle);
		bufferInfo.setOffset(0);
		bufferInfo.setRange(sizeof(UniformBufferData));

		vk::WriteDescriptorSet write;
		write.setDstSet(m_descriptorSets[i]);
		write.setDstBinding(0);
		write.setDstArrayElement(0);
		write.setDescriptorType(vk::DescriptorType::eUniformBuffer);
		write.setDescriptorCount(1);
		write.setPBufferInfo(&bufferInfo);

		m_device.updateDescriptorSets(1, &write, 0, nullptr);
	}
}

void PipelineDescriptor::addResource(vk::DescriptorType type, uint32_t count, vk::ShaderStageFlags stage)
{
	vk::DescriptorSetLayoutBinding layoutBinding;
	layoutBinding.setBinding(m_currentBinding++);
	layoutBinding.setDescriptorType(type);
	layoutBinding.setDescriptorCount(count);
	layoutBinding.setStageFlags(stage);

	m_layoutBindings.push_back(layoutBinding);

	uint32_t numSets = count * Renderer::getFramesInFlight();
	vk::DescriptorPoolSize poolSize;
	poolSize.setType(type);
	poolSize.setDescriptorCount(numSets);

	m_poolSizes.push_back(poolSize);
	m_maxSets = glm::max(m_maxSets, numSets);

	m_debugInfo.emplace_back(std::make_tuple(type, count, stage));
}

void PipelineDescriptor::printDebugInfo()
{
	Logging::Info("\t\t--displaying layout info--");
	Logging::Info("\t\t\tframes in flight: {}", Renderer::getFramesInFlight());
	Logging::Info("\ttype\tcount\tstage");
	Logging::Info("---------------------------------------------");
	for (auto& tuple : m_debugInfo)
	{
		vk::DescriptorType type;
		uint32_t count;
		vk::ShaderStageFlags stage;
		std::tie(type, count, stage) = tuple;
		// fix this
		//  Logging::Info("\t{}\t{}\t{}", type, count, stage);
	}
}
