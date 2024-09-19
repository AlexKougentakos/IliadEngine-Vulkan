#pragma once
#include "Window.h"

#include "../Graphics/Device.h"
#include "../Graphics/SwapChain.h"
#include "Graphics/Model.h"

namespace ili
{
	class Renderer
	{
	public:
		Renderer(Window& window, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
		VkRenderPass GetSwapChainRenderPass() const { return m_pSwapChain->GetRenderPass(); }

		bool IsFrameInProgress() const { return m_FrameStarted; }

		int GetFrameIndex() const 
		{
			assert(m_FrameStarted && "Cannot get command buffer when frame not in progress.");
			return m_CurrentFrameIndex; 
		}

		VkCommandBuffer GetCurrentCommandBuffer() const 
		{
			assert(m_FrameStarted && "Cannot get command buffer when frame not in progress.");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();

		void RecreateSwapChain();

		Window& m_Window;
		Device& m_Device;
		std::unique_ptr<SwapChain> m_pSwapChain{};
		std::vector<VkCommandBuffer> m_CommandBuffers{};

		uint32_t m_CurrentImageIndex{ 0 };
		int m_CurrentFrameIndex{ 0 };
		bool m_FrameStarted{ false };
	};
}
