#include "PointLightSystem.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "glm/gtc/constants.hpp"

#include "../SceneGraph/GameObject.h"
#include "../SceneGraph/Camera.h"
#include "Structs/FrameInfo.h"

namespace ili
{
	struct PointLightPushConstants
	{
		glm::vec4 position{};
		glm::vec4 color{};
		float radius{};
	};


	PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

		const std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};

		Pipeline::GetDefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.vertexAttributeDescriptions.clear();
		pipelineConfig.vertexBindingDescriptions.clear();

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(m_Device, "Assets/CompiledShaders/pointLight.vert.spv", "Assets/CompiledShaders/pointLight.frag.spv", pipelineConfig);
	}

	void PointLightSystem::Update(const FrameInfo& frameInfo, GlobalUbo& ubo, std::vector<GameObject>& gameObjects)
	{
		int lightIndex{};
		for (auto& obj : gameObjects)
		{
			if (!obj.GetPointLightComponent()) continue;

			ubo.pointLights[lightIndex].position = glm::vec4(obj.GetTransform().position, 1.f);
			ubo.pointLights[lightIndex].color = glm::vec4(obj.GetColor(), obj.GetPointLightComponent()->lightIntensity);

			lightIndex++;
		}

		ubo.pointLightCount = lightIndex;
	}

	void PointLightSystem::Render(const FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		m_Pipeline->Bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1,
			&frameInfo.globalDescriptorSet, 0, nullptr);

		for (auto& obj : gameObjects)
		{
			if (!obj.GetPointLightComponent()) return;

			PointLightPushConstants pushConstants{};
			pushConstants.position = glm::vec4(obj.GetTransform().position, 1.f);
			pushConstants.color = glm::vec4(obj.GetColor(), obj.GetPointLightComponent()->lightIntensity);
			pushConstants.radius = obj.GetTransform().scale.x;

			vkCmdPushConstants(frameInfo.commandBuffer, 
				m_PipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(PointLightPushConstants), 
				&pushConstants);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
	}

}