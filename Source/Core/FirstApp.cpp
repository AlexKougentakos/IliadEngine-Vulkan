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

	std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) 
	{
		Model::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) 
		{
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<Model>(device, modelBuilder);
	}

	void FirstApp::LoadGameObjects()
	{
		const std::shared_ptr<Model> cubeModel = createCubeModel(m_Device, { 0.f, 0.f, 0.f });

		auto cube = GameObject::Create();
		cube.SetModel(cubeModel);
		cube.GetTransform().position = { 0.f, 0.f, 2.5f };
		cube.GetTransform().rotation = { 0.f, 0.f, 0.f };
		cube.GetTransform().scale = { .5f, .5f, .5f };

		m_GameObjects.push_back(cube);
	}
}
