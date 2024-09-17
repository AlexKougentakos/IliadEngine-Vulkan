#include <iostream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "FirstApp.h"
#include "imgui.h"
#include "Window.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"
#include "glm/fwd.hpp"
#include "glm/vec4.hpp"

int main()
{
	ili::FirstApp app{};

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

    return 0;
}
