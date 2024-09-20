#include "FirstApp.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <chrono>
#include <glm/glm.hpp>

#include "RenderSystem.h"
#include "../SceneGraph/GameObject.h"
#include "glm/gtc/constants.hpp"
#include "Input/KeyboardInputMovement.h"
#include "Structs/FrameInfo.h"

namespace ili
{
	struct GlobalUbo
	{
		glm::mat4 projectionViewMatrix{1.f};
		glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
	};

	FirstApp::FirstApp() : m_Window(WIDTH, HEIGHT, "Iliad Engine - Vulkan")
	{
		m_GlobalDescriptorPool = DescriptorPool::Builder(m_Device)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
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
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
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
		Camera camera{};
		
		camera.SetViewTarget({ 0.f, 0.f, 0.f }, { 0.5f, 0.f, 1.f });

		auto viewerObject = GameObject::Create();
		KeyboardMovementController cameraController{};


		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!m_Window.ShouldClose())
		{
			glfwPollEvents();

			//NOTE: Do this after the poll events as it may block (like during resizing)
			const auto newTime = std::chrono::high_resolution_clock::now();
			const auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.MoveInPlaneXZ(m_Window.GetWindow(), frameTime, viewerObject);
			camera.SetViewYXZ(viewerObject.GetTransform().position, viewerObject.GetTransform().rotation);

			//Placed inside the loop so that it is kept up to date when the window is resized
			const float aspectRation = m_Renderer.GetAspectRatio();
			//camera.SetOrthographicProjection(-aspectRation, aspectRation, -1, 1, -1, 1);
			camera.SetPerspectiveProjection(glm::radians(60.f), aspectRation, 0.1f, 10.f);

			if (const auto commandBuffer = m_Renderer.BeginFrame())
			{
				const int frameIndex = m_Renderer.GetFrameIndex();
				const FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

				GlobalUbo globalUbo{};
				globalUbo.projectionViewMatrix = camera.GetProjection() * camera.GetView();
				uboBuffers[frameIndex]->WriteToIndex(&globalUbo, frameIndex);
				uboBuffers[frameIndex]->Flush();

				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				renderSystem.RenderGameObjects(frameInfo, m_GameObjects);
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	void FirstApp::LoadGameObjects()
	{
		const std::shared_ptr<Model> cubeModel = Model::CreateModelFromFile(m_Device, "Assets/Models/bunny.obj");

		auto cube = GameObject::Create();
		cube.SetModel(cubeModel);
		cube.GetTransform().position = { 0.f, 0.f, .5f };
		cube.GetTransform().rotation = { 0.f, 180.f, 0.f };
		cube.GetTransform().scale = { .5f, -.5f, .5f };

		const std::shared_ptr<Model> vase = Model::CreateModelFromFile(m_Device, "Assets/Models/flat_vase.obj");
		auto vaseObject = GameObject::Create();
		vaseObject.SetModel(vase);
		vaseObject.GetTransform().position = { 0.5f, 0.f, 0.5f };
		vaseObject.GetTransform().rotation = { 0.f, 0.f, 0.f };
		vaseObject.GetTransform().scale = { 1.f, .5f, 1.f };

		m_GameObjects.emplace_back(cube);
		m_GameObjects.emplace_back(vaseObject);
	}
}
