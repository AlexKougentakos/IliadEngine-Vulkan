#include "FirstApp.h"



namespace ili
{
	FirstApp::FirstApp() : m_Window(WIDTH, HEIGHT, "Iliad Engine - Vulkan")
	{
	}

	FirstApp::~FirstApp()
	{
	}

	void FirstApp::Run()
	{
		while (!m_Window.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}