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

namespace ili
{


	FirstApp::FirstApp() : m_Window(WIDTH, HEIGHT, "Iliad Engine - Vulkan")
	{
		LoadGameObjects();
	}

	FirstApp::~FirstApp()
	{
		
	}

	void FirstApp::Run()
	{
		RenderSystem renderSystem(m_Device, m_Renderer.GetSwapChainRenderPass());
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
				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				renderSystem.RenderGameObjects(commandBuffer, m_GameObjects, camera);
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
		vaseObject.GetTransform().scale = { 1.f, 1.f, 1.f };

		m_GameObjects.emplace_back(cube);
		m_GameObjects.emplace_back(vaseObject);
	}
}
