#pragma once
#include "Window.h"

#include "../Graphics/Pipeline.h"
#include "../Graphics/Device.h"
#include "../Graphics/SwapChain.h"
#include "Graphics/Model.h"
#include "SceneGraph/GameObject.h"

namespace ili
{
	class FirstApp
	{
	public:
		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		FirstApp(FirstApp&&) = delete;
		FirstApp& operator=(FirstApp&&) = delete;

		void Run();

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();

		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIdx);
		
		void FreeCommandBuffers();

		void LoadGameObjects();
		void RenderGameObjects(VkCommandBuffer commandBuffer);

		void DrawFrame();

		Window m_Window;
		Device m_Device{ m_Window };
		std::unique_ptr<SwapChain> m_pSwapChain{};
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout{};
		std::vector<VkCommandBuffer> m_CommandBuffers{};

		std::vector<GameObject> m_GameObjects{};
	};
}
