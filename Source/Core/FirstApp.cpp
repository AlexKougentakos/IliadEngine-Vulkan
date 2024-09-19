#include "FirstApp.h"

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
		glm::mat2 transform{ 1.f }; //Identity matrix
		glm::vec2 offset{};
		alignas(16) glm::vec3 color{};
		//Use alignas because a three component item needs to be equal to 4 times its scalar alignment size
		//Aka multiples of 2 of the original size. In this case, 4 bytes for a float, so 4 * 4 = 16
	};

	FirstApp::FirstApp() : m_Window(WIDTH, HEIGHT, "Iliad Engine - Vulkan")
	{
		LoadGameObjects();
		CreatePipelineLayout();
		CreatePipeline();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);	
	}

	void FirstApp::Run()
	{
		while (!m_Window.ShouldClose())
		{
			glfwPollEvents();
			

			const auto commandBuffer = m_Renderer.BeginFrame();
			if (commandBuffer)
			{
				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				RenderGameObjects(commandBuffer);
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	void FirstApp::CreatePipelineLayout()
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

	void FirstApp::CreatePipeline()
	{
		PipelineConfigInfo pipelineConfig{};

		Pipeline::GetDefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = m_Renderer.GetSwapChainRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(m_Device, "Assets/CompiledShaders/shader.vert.spv", "Assets/CompiledShaders/shader.frag.spv", pipelineConfig);
	}


	void FirstApp::LoadGameObjects()
	{
		std::vector<Model::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.f, 0.f, 0.f}},
			{{0.5f, 0.5f}, {0.f, 1.f, 0.f}},
			{{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}
		};

		const auto model = std::make_shared<Model>(m_Device, vertices);

		for (int i{}; i < 10; i++)
		{
			auto triangle = GameObject::Create();
			triangle.SetModel(model);
			//Randomize the color
			triangle.SetColor({ static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX });
			triangle.GetTransform().scale = { 0.3f * i, .3f * i };

			m_GameObjects.push_back(std::move(triangle));
		}
	}

	void FirstApp::RenderGameObjects(VkCommandBuffer commandBuffer)
	{
		int i = 0;
		for (auto& object : m_GameObjects)
		{
			i += 1;
			object.GetTransform().rotation = glm::mod<float>(object.GetTransform().rotation + 0.001f, glm::two_pi<float>());
		}

		m_Pipeline->Bind(commandBuffer);
		for (const auto& gameObject : m_GameObjects)
		{
			SimplePushConstantData pushData{};
			pushData.offset = gameObject.GetTransformConst().position;
			pushData.color = gameObject.GetColor();
			pushData.transform = gameObject.GetTransformConst().GetMatrix();

			vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &pushData);

			gameObject.GetModel()->Bind(commandBuffer);
			gameObject.GetModel()->Draw(commandBuffer);
		}
	}
}
