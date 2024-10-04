#include <chrono>
#include <iostream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "PointLightSystem.h"
#include "Window.h"
#include "Structs/FrameInfo.h"

#include "Graphics/Device.h"
#include "Renderer.h"
#include "RenderSystem.h"
#include "Graphics/Descriptors.h"
#include "Input/KeyboardInputMovement.h"
#include "SceneGraph/SceneManager.h"

#include "IliadGame.h"

#include "SceneGraph/TransformComponent.h"

namespace ili
{

	void IliadGame::Run()
	{
		// PREPARING
		OnGamePreparing();

		// INITIALIZE
		InitializeWindow();
		InitializeVulkan();

		ContentLoader::GetInstance().Initialize(m_Device.get());

		InitializeGame();

		// Set camera view target
		m_Camera.SetViewTarget({ 0.f, 0.f, 0.f }, { 0.5f, 0.f, 1.f });

		// Create viewer object
		const auto viewerObject = m_pCurrentScene->CreateGameObject<GameObject>();
		viewerObject->GetTransform()->SetPosition({ 0.f, 0.f, -1.f });

		// Initialize current time
		m_CurrentTime = std::chrono::high_resolution_clock::now();

		while (!m_Window->ShouldClose())
		{
			glfwPollEvents();

			GameLoop(viewerObject, m_CameraController);
		}

		vkDeviceWaitIdle(m_Device->GetDevice());
	}

	void IliadGame::GameLoop(GameObject* viewerObject, KeyboardMovementController& cameraController)
	{
		// Calculate deltaTime
		const auto newTime = std::chrono::high_resolution_clock::now();
		const auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - m_CurrentTime).count();
		m_CurrentTime = newTime;

		// Handle input and update GameObject
		cameraController.MoveInPlaneXZ(m_Window->GetWindow(), frameTime, *viewerObject);
		m_Camera.SetViewYXZ(viewerObject->GetTransform()->GetPosition(), viewerObject->GetTransform()->GetRotationRadians());

		// Update camera projection
		const float aspectRatio = m_Renderer->GetAspectRatio();
		m_Camera.SetPerspectiveProjection(glm::radians(60.f), aspectRatio, 0.1f, 10.f);

		// Begin rendering
		if (const auto commandBuffer = m_Renderer->BeginFrame())
		{
			const int frameIndex = m_Renderer->GetFrameIndex();
			m_FramePools[frameIndex]->ResetPool();
			const FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, m_Camera, m_GlobalDescriptorSets[frameIndex], *m_FramePools[frameIndex] };

			GlobalUbo globalUbo{};
			globalUbo.projectionMatrix = m_Camera.GetProjection();
			globalUbo.viewMatrix = m_Camera.GetView();
			globalUbo.inverseViewMatrix = m_Camera.GetInverseView();
			m_SceneManager.Update(frameTime);
			m_PointLightSystem.value().Update(frameInfo, globalUbo, m_pCurrentScene->GetPointLights());
			m_UboBuffers[frameIndex]->WriteToBuffer(&globalUbo);
			m_UboBuffers[frameIndex]->Flush();

			m_Renderer->BeginSwapChainRenderPass(commandBuffer);
			m_TextureRenderSystem.value().RenderGameObjects(frameInfo, m_pCurrentScene->GetGameObjects());
			m_RenderSystem.value().RenderGameObjects(frameInfo, m_pCurrentScene->GetGameObjects());
			m_PointLightSystem.value().Render(frameInfo, m_pCurrentScene->GetPointLights());
			m_Renderer->EndSwapChainRenderPass(commandBuffer);
			m_Renderer->EndFrame();
		}
	}

	void IliadGame::InitializeWindow()
	{
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		m_Window = std::make_unique<Window>(WIDTH, HEIGHT, "Iliad Game");

		// Hide and capture the cursor
		glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Set initial cursor position to the center of the window
		int windowWidth, windowHeight;
		glfwGetWindowSize(m_Window->GetWindow(), &windowWidth, &windowHeight);
		glfwSetCursorPos(m_Window->GetWindow(), windowWidth / 2.0, windowHeight / 2.0);
	}

	void IliadGame::InitializeVulkan()
	{
		m_Device = std::make_unique<Device>(m_Window.get());
		m_Renderer = std::make_unique<Renderer>(m_Window.get(), *m_Device);

		m_GlobalDescriptorPool = DescriptorPool::Builder(*m_Device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		m_FramePools.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		const auto framePoolBuilder = DescriptorPool::Builder(*m_Device)
			.setMaxSets(1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
			.setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

		for (int i{}; i < m_FramePools.size(); ++i)
		{
			m_FramePools[i] = framePoolBuilder.build();
		}

		for (int i{}; i < m_UboBuffers.size(); ++i)
		{
			m_UboBuffers[i] = std::make_unique<Buffer>(*m_Device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_Device->Properties.limits.minUniformBufferOffsetAlignment);
			m_UboBuffers[i]->Map();
		}

		const auto globalSetLayout = DescriptorSetLayout::Builder(*m_Device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build();

		for (int i{}; i < m_GlobalDescriptorSets.size(); ++i)
		{
			auto bufferInfo = m_UboBuffers[i]->DescriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_GlobalDescriptorPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(m_GlobalDescriptorSets[i]);
		}

		m_RenderSystem.emplace(*m_Device, m_Renderer->GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());

		m_PointLightSystem.emplace(*m_Device, m_Renderer->GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());

		m_TextureRenderSystem.emplace(*m_Device, m_Renderer->GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
	}

}
