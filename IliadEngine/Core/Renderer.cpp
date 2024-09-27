#include "Renderer.h"

#include <array>
#include <stdexcept>

#include "../SceneGraph/GameObject.h"

namespace ili
{
	Renderer::Renderer(Window* window, Device& device) : m_Window(window), m_Device(device)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		FreeCommandBuffers();
	}

	VkCommandBuffer Renderer::BeginFrame()
	{
		assert(!m_FrameStarted && "Cannot call BeginFrame while frame is in progress");

		const auto result = m_pSwapChain->AcquireNextImage(&m_CurrentImageIndex);

		//Used to recreate swap chain if window is resized
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire next image");
		}

		m_FrameStarted = true;

		const auto commandBuffer = GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void Renderer::EndFrame()
	{
		assert(m_FrameStarted && "Cannot call EndFrame while frame is not in progress");
		const auto commandBuffer = GetCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}

		const auto result = m_pSwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->WasWindowResized())
		{
			m_Window->ResetWindowResizedFlag();
			RecreateSwapChain();
		}

		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit command buffer");
		}

		m_FrameStarted = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_FrameStarted && "Cannot begin render pass when frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "You can only begin the render pass on a command buffer from the same frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_pSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_pSwapChain->GetFrameBuffer(m_CurrentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_pSwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_pSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_pSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		const VkRect2D scissor{ {0, 0}, m_pSwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_FrameStarted && "Cannot begin render pass when frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "You can only end the render pass on a command buffer from the same frame");


		vkCmdEndRenderPass(commandBuffer);
	}

	void Renderer::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

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

	void Renderer::RecreateSwapChain()
	{
		auto extent = m_Window->GetExtent();

		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_Window->GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device.GetDevice());

		if (!m_pSwapChain) 
		{
			m_pSwapChain = std::make_unique<SwapChain>(m_Device, extent);
		}
		else 
		{
			std::shared_ptr<SwapChain> oldSwapChain = std::move(m_pSwapChain);
			m_pSwapChain = std::make_unique<SwapChain>(m_Device, extent, oldSwapChain);
			if (!oldSwapChain->CompareSwapFormats(*m_pSwapChain)) 
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device.GetDevice(), m_Device.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}
}