﻿#pragma once
#include "Window.h"

#include "../Graphics/Pipeline.h"
#include "../Graphics/Device.h"
#include "Renderer.h"
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

		void LoadGameObjects();
		void RenderGameObjects(VkCommandBuffer commandBuffer);

		Window m_Window;
		Device m_Device{ m_Window };
		Renderer m_Renderer{ m_Window, m_Device };
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout{};

		std::vector<GameObject> m_GameObjects{};
	};
}
