#pragma once

#include "Graphics/Buffer.h"
#include "SceneGraph/Camera.h"

namespace ili
{
	struct FrameInfo final
	{
		int frameIndex{};
		float frameTime{};
		VkCommandBuffer commandBuffer{};
		Camera& camera;
		VkDescriptorSet globalDescriptorSet{};
	};
}