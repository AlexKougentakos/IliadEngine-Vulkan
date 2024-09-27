#include "FirstApp.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <chrono>
#include <glm/glm.hpp>

#include "PointLightSystem.h"
#include "RenderSystem.h"
#include "../SceneGraph/GameObject.h"
#include "glm/gtc/constants.hpp"
#include "Input/KeyboardInputMovement.h"
#include "Structs/FrameInfo.h"

namespace ili
{
	FirstApp::FirstApp() : m_Window(WIDTH, HEIGHT, "Iliad Engine - Vulkan")
	{
		m_GlobalDescriptorPool = DescriptorPool::Builder(m_Device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		LoadGameObjects();
	}

	FirstApp::~FirstApp()
	{
		
	}

	void FirstApp::Run()
	{
		std::vector<std::unique_ptr<Buffer>> uboBuffers{SwapChain::MAX_FRAMES_IN_FLIGHT};

		for (int i{}; i < uboBuffers.size(); ++i)
		{
			uboBuffers[i] = std::make_unique<Buffer>(m_Device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_Device.Properties.limits.minUniformBufferOffsetAlignment);
			uboBuffers[i]->Map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(m_Device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build();

		std::vector<VkDescriptorSet> globalDescriptorSets{ SwapChain::MAX_FRAMES_IN_FLIGHT };
		for (int i{}; i < globalDescriptorSets.size(); ++i)
		{
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_GlobalDescriptorPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		RenderSystem renderSystem(m_Device, m_Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		PointLightSystem pointLightSystem(m_Device, m_Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		Camera camera{};
		
		camera.SetViewTarget({ 0.f, 0.f, 0.f }, { 0.5f, 0.f, 1.f });

		auto viewerObject = m_Scene.CreateGameObject();
		viewerObject->GetTransform().position = { 0.f, 0.f, -1.f };
		KeyboardMovementController cameraController{};


		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!m_Window.ShouldClose())
		{
			glfwPollEvents();

			//NOTE: Do this after the poll events as it may block (like during resizing)
			const auto newTime = std::chrono::high_resolution_clock::now();
			const auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.MoveInPlaneXZ(m_Window.GetWindow(), frameTime, *viewerObject);
			camera.SetViewYXZ(viewerObject->GetTransform().position, viewerObject->GetTransform().rotation);

			//Placed inside the loop so that it is kept up to date when the window is resized
			const float aspectRation = m_Renderer.GetAspectRatio();
			//camera.SetOrthographicProjection(-aspectRation, aspectRation, -1, 1, -1, 1);
			camera.SetPerspectiveProjection(glm::radians(60.f), aspectRation, 0.1f, 10.f);

			if (const auto commandBuffer = m_Renderer.BeginFrame())
			{
				const int frameIndex = m_Renderer.GetFrameIndex();
				const FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

				GlobalUbo globalUbo{};
				globalUbo.projectionMatrix = camera.GetProjection();
				globalUbo.viewMatrix = camera.GetView();
				globalUbo.inverseViewMatrix = camera.GetInverseView();
				pointLightSystem.Update(frameInfo, globalUbo, m_Scene.GetGameObjects());
				uboBuffers[frameIndex]->WriteToBuffer(&globalUbo);
				uboBuffers[frameIndex]->Flush();

				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				renderSystem.RenderGameObjects(frameInfo, m_Scene.GetGameObjects());
				pointLightSystem.Render(frameInfo, m_Scene.GetGameObjects());
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	void FirstApp::LoadGameObjects()
	{
		const std::shared_ptr<Model> bunnyModel = Model::CreateModelFromFile(m_Device, "Assets/Models/bunny.obj");

		auto bunny1 = m_Scene.CreateGameObject();
		bunny1->SetModel(bunnyModel);
		bunny1->GetTransform().position = { -0.5f, 0.f, 0.f };
		bunny1->GetTransform().rotation = { 0.f, 180.f, 0.f };
		bunny1->GetTransform().scale = { 0.5f, -0.5f, 0.5f };


		// Second bunny
		auto bunny2 = m_Scene.CreateGameObject();
		bunny2->SetModel(bunnyModel);
		bunny2->GetTransform().position = { 0.5f, 0.f, 0.f }; // Position it to the right of the light
		bunny2->GetTransform().rotation = { 0.f, 180.f, 0.f };
		bunny2->GetTransform().scale = { 0.5f, -0.5f, 0.5f };

		// Light in the middle of the two bunnies
		//GameObject pointLightComponent = GameObject::MakePointLight(0.2f);
		//pointLightComponent.GetTransform().position = { 0.f, -0.5f, 0.f }; // Slightly above ground level
		//pointLightComponent.SetColor({ 1.f, 0.f, 0.f });

		// Example vase and floor objects (optional, retained for context)
		const std::shared_ptr<Model> vase = Model::CreateModelFromFile(m_Device, "Assets/Models/flat_vase.obj");
		auto vaseObject = m_Scene.CreateGameObject();
		vaseObject->SetModel(vase);
		vaseObject->GetTransform().position = { 1.0f, 0.f, 0.0f };
		vaseObject->GetTransform().rotation = { 0.f, 0.f, 0.f };
		vaseObject->GetTransform().scale = { 1.f, 0.5f, 1.f };

		const std::shared_ptr<Model> floor = Model::CreateModelFromFile(m_Device, "Assets/Models/quad.obj");
		auto floorObject = m_Scene.CreateGameObject();
		floorObject->SetModel(floor);
		floorObject->GetTransform().position = { 0.5f, 0.f, 0.0f };
		floorObject->GetTransform().rotation = { 0.f, 0.f, 0.f };
		floorObject->GetTransform().scale = { 3.f, 0.5f, 3.f };
	}
}