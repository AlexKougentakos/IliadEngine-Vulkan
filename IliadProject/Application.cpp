#include <chrono>
#include <iostream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "Core/PointLightSystem.h"
#include "Core/Window.h"
#include "Structs/FrameInfo.h"

#include "Graphics/Device.h"
#include "Core/Renderer.h"
#include "Core/RenderSystem.h"
#include "Graphics/Descriptors.h"
#include "Input/KeyboardInputMovement.h"
#include "ProjectCore/IliadSampleProject.h"
#include "SceneGraph/SceneManager.h"
int main()
{
	const auto pGame = new IliadSampleProject();
	pGame->Run();
	delete pGame;
	return 0;
}