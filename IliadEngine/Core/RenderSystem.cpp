#include "RenderSystem.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "glm/gtc/constants.hpp"

#include "../SceneGraph/GameObject.h"
#include "../SceneGraph/Camera.h"
#include "SceneGraph/ModelComponent.h"
#include "SceneGraph/TransformComponent.h"
#include "Structs/FrameInfo.h"

namespace ili
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.f }; //Identity matrix
		glm::mat4 normalMatrix{ 1.f }; //Identity matrix
	};

	RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

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

	void RenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};

		Pipeline::GetDefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(m_Device, "Assets/CompiledShaders/shader.vert.spv", "Assets/CompiledShaders/shader.frag.spv", pipelineConfig);
	}

	void RenderSystem::RenderGameObjects(const FrameInfo& frameInfo, const std::vector<std::unique_ptr<GameObject>>& gameObjects)
	{
		m_Pipeline->Bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, 
			&frameInfo.globalDescriptorSet, 0, nullptr);

		for (auto& gameObject : gameObjects)
		{
			const auto modelComponent = gameObject->GetComponent<ModelComponent>();

			if (!modelComponent) continue;

//			if (!modelComponent->GetModel() || modelComponent->GetDiffuseMap()) continue;

			SimplePushConstantData pushData{};

			pushData.modelMatrix = gameObject->GetTransform()->GetMatrix();
			pushData.normalMatrix = gameObject->GetTransform()->GetNormalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &pushData);


			modelComponent->GetModel()->Bind(frameInfo.commandBuffer);
			modelComponent->GetModel()->Draw(frameInfo.commandBuffer);
		}
	}
}