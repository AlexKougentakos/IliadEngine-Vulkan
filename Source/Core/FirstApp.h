﻿#pragma once
#include "Window.h"

#include "Graphics/Device.h"
#include "Renderer.h"
#include "Graphics/Descriptors.h"
#include "SceneGraph/SceneManager.h"

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
		void LoadGameObjects();

		Window m_Window;
		Device m_Device{ m_Window };
		Renderer m_Renderer{ m_Window, m_Device };
		SceneManager m_SceneManager{};

		std::unique_ptr<DescriptorPool> m_GlobalDescriptorPool{};
		Scene* m_pCurrentScene{ nullptr };
	};
}
