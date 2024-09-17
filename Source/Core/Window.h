#pragma once
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <string>

#include "GLFW/glfw3.h"

namespace ili
{
	class Window
	{
	public:
		Window(const int width, const int height, const std::string& name);
		~Window();

		bool ShouldClose() const { return glfwWindowShouldClose(m_pWindow); }
	private:
		void InitializeWindow();

		GLFWwindow* m_pWindow{};

		std::string m_Title{};
		int m_Width{};
		int m_Height{};
	};
}
