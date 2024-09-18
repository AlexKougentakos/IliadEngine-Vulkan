#include "FirstApp.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "../SceneGraph/GameObject.h"
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
		RecreateSwapChain();
		CreateCommandBuffers();
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
			DrawFrame();
		}

		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	void FirstApp::DrawFrame()
	{
		uint32_t imageIndex{};
		auto result = m_pSwapChain->AcquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire next image");
		}

		RecordCommandBuffer(imageIndex);

		result = m_pSwapChain->SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasWindowResized())
		{
			m_Window.ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit command buffer");
		}
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

		pipelineConfig.renderPass = m_pSwapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(m_Device, "Assets/CompiledShaders/shader.vert.spv", "Assets/CompiledShaders/shader.frag.spv", pipelineConfig);
	}

	void FirstApp::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_pSwapChain->ImageCount());

		VkCommandBufferAllocateInfo commandBufferAllocInfo{};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //Can be submitted to a queue for execution, but cannot be called from other command buffers
		commandBufferAllocInfo.commandPool = m_Device.GetCommandPool();
		commandBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.GetDevice(), &commandBufferAllocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void FirstApp::RecreateSwapChain()
	{
		auto extent = m_Window.GetExtent();

		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_Window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device.GetDevice());

		if (m_pSwapChain == nullptr)
		{
			m_pSwapChain = std::make_unique<SwapChain>(m_Device, extent);
		}
		else
		{
			m_pSwapChain = std::make_unique<SwapChain>(m_Device, extent, std::move(m_pSwapChain));
			if (m_pSwapChain->ImageCount() != m_CommandBuffers.size())
			{
				CreateCommandBuffers();
			}
		}
		CreatePipeline();
	}

	void FirstApp::RecordCommandBuffer(int imageIdx)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		const auto currentCommandBuffer = m_CommandBuffers[imageIdx];

		if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_pSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_pSwapChain->GetFrameBuffer(imageIdx);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_pSwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_pSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_pSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{ {0, 0}, m_pSwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);


		RenderGameObjects(currentCommandBuffer);

		vkCmdEndRenderPass(currentCommandBuffer);

		if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}
	}

	void FirstApp::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device.GetDevice(), m_Device.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
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

		auto triangle = GameObject::Create();
		triangle.SetModel(model);
		triangle.SetColor({ .1f, .8f, .1f });
		triangle.Translate( {.2f, 0.f});

		m_GameObjects.push_back(std::move(triangle));
	}

	void FirstApp::RenderGameObjects(VkCommandBuffer commandBuffer)
	{
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
