#pragma once
#include <string>

struct GameContext
{
	unsigned int windowWidth{ 1280 };
	unsigned int windowHeight{ 720 };
	std::string windowTitle{ "Iliad Engine - Vulkan" };

	//Todo: use this
	std::string contentDirectory{ "Content" };
};

struct SceneContext
{
	
};