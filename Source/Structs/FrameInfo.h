#pragma once

#include "Graphics/Buffer.h"
#include "SceneGraph/Camera.h"

namespace ili
{
	#define MAX_POINT_LIGHTS 10

	struct PointLight
	{
		glm::vec4 position{}; //Ignore the w component, it's just there for padding
		glm::vec4 color{}; //Alpha channel is intensity
	};

	struct GlobalUbo
	{
		glm::mat4 projectionMatrix{ 1.f };
		glm::mat4 viewMatrix{ 1.f };
		glm::mat4 inverseViewMatrix{ 1.f };

		glm::vec4 ambientColor{ 1.f, 1.f, 1.f, 0.02f }; //Alpha channel is intensity
		
		PointLight pointLights[MAX_POINT_LIGHTS];
		int pointLightCount{};

	};

	struct FrameInfo final
	{
		int frameIndex{};
		float frameTime{};
		VkCommandBuffer commandBuffer{};
		Camera& camera;
		VkDescriptorSet globalDescriptorSet{};
	};
}