#include "RenderSystem.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "../SceneGraph/GameObject.h"
#include "glm/gtc/constants.hpp"

namespace ili
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.f }; //Identity matrix
		alignas(16) glm::vec3 color{};
		//Use alignas because a three component item needs to be equal to 4 times its scalar alignment size
		//Aka multiples of 2 of the original size. In this case, 4 bytes for a float, so 4 * 4 = 16
	};

	RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass) : m_Device(device)
	{
		CreatePipelineLayout();
		CreatePipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void RenderSystem::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);



		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
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

	void RenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects)
	{
		int i = 0;
		for (auto& object : gameObjects)
		{
			i += 1;
			object.GetTransform().rotation.y = glm::mod<float>(object.GetTransform().rotation.y + 0.01f, glm::two_pi<float>());
			object.GetTransform().rotation.x = glm::mod<float>(object.GetTransform().rotation.x + 0.01f, glm::two_pi<float>());
		}

		m_Pipeline->Bind(commandBuffer);
		for (const auto& gameObject : gameObjects)
		{
			SimplePushConstantData pushData{};
			pushData.color = gameObject.GetColor();
			pushData.transform = gameObject.GetTransformConst().GetMatrix();

			vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &pushData);

			gameObject.GetModel()->Bind(commandBuffer);
			gameObject.GetModel()->Draw(commandBuffer);
		}
	}
}
