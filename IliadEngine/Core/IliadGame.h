#pragma once

#include "Input/KeyboardInputMovement.h"
#include "SceneGraph/SceneManager.h"
#include "SceneGraph/Camera.h"
#include "Graphics/Device.h"
#include "Core/Renderer.h"

#include "Graphics/Descriptors.h"
#include "Core/RenderSystem.h"
#include "Core/PointLightSystem.h"


#include <chrono>


namespace ili
{

	class IliadGame
	{
	public:
		IliadGame() = default;
		virtual ~IliadGame() = default;
		IliadGame(const IliadGame& other) = delete;
		IliadGame(IliadGame&& other) noexcept = delete;
		IliadGame& operator=(const IliadGame& other) = delete;
		IliadGame& operator=(IliadGame&& other) noexcept = delete;

		void Run();

	protected:
		//Called before all the initializations, in case it is ever needed
		virtual void OnGamePreparing() = 0;
		virtual void InitializeGame() = 0;
		void GameLoop(GameObject* viewerObject, KeyboardMovementController cameraController);

	//private:
		void InitializeWindow();
		void InitializeVulkan();

		std::unique_ptr<Window> m_Window{};
		std::unique_ptr<Device> m_Device{};
		std::unique_ptr<Renderer> m_Renderer{};

		//todo: remove this from here
		Camera m_Camera{};

		//Timing Stuff
		std::chrono::steady_clock::time_point m_CurrentTime{};

		//Vulkan stuff
		std::unique_ptr<DescriptorPool> m_GlobalDescriptorPool{};
		std::vector<std::unique_ptr<ili::Buffer>> m_UboBuffers{ ili::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::vector<VkDescriptorSet> m_GlobalDescriptorSets{ ili::SwapChain::MAX_FRAMES_IN_FLIGHT };

		std::optional<RenderSystem> m_RenderSystem{};
		std::optional<PointLightSystem> m_PointLightSystem{};

	protected:
		//These have to be here, due to order of destruction. Items in the scene need to be destroyed before the window, device & renderer
		//I can also just make them all pointers and manage their destruction order.
		SceneManager m_SceneManager{};
		Scene* m_pCurrentScene{ nullptr };

	};
}
