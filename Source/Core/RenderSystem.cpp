#include "RenderSystem.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "glm/gtc/constants.hpp"

#include "../SceneGraph/GameObject.h"
#include "../SceneGraph/Camera.h"

namespace ili
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.f }; //Identity matrix
		glm::mat4 normalMatrix{ 1.f }; //Identity matrix
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

	void RenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera)
	{
		m_Pipeline->Bind(commandBuffer);

		//Every rendered object will use the same projection and view matrix, so we don't need to do the calculation for each object
		const auto projectionView = camera.GetProjection() * camera.GetView();

		for (const auto& gameObject : gameObjects)
		{
			auto modelMatrix = gameObject.GetTransformConst().GetMatrix();

			SimplePushConstantData pushData{};
			pushData.transform = projectionView * modelMatrix;
			pushData.normalMatrix = gameObject.GetTransformConst().GetNormalMatrix();

			vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &pushData);

			gameObject.GetModel()->Bind(commandBuffer);
			gameObject.GetModel()->Draw(commandBuffer);
		}
	}
}
