#include "Window.h"

#include <stdexcept>

namespace ili
{
	Window::Window(const int width, const int height, const std::string& name) 
		: m_Width(width), m_Height(height), m_Title(name)
	{
		InitializeWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const
	{
		if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void Window::InitializeWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_pWindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
	}
}
