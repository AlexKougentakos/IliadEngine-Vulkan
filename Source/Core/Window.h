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

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;


		bool ShouldClose() const { return glfwWindowShouldClose(m_pWindow); }
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const;

		VkExtent2D GetExtent() const { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }

		bool WasWindowResized() const { return  m_WindowResized; }
		void ResetWindowResizedFlag() { m_WindowResized = false; }

	private:
		void InitializeWindow();
		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

		GLFWwindow* m_pWindow{};

		std::string m_Title{};
		int m_Width{};
		int m_Height{};

		bool m_WindowResized{false};
	};
}
