#pragma once

#include "../Graphics/Pipeline.h"
#include "../Graphics/Device.h"
#include "SceneGraph/GameObject.h"

namespace ili
{
	class RenderSystem
	{
	public:
		RenderSystem(Device& device, VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;
		RenderSystem(RenderSystem&&) = delete;
		RenderSystem& operator=(RenderSystem&&) = delete;

		void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects);
	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;

		std::unique_ptr<Pipeline> m_Pipeline{};
		VkPipelineLayout m_PipelineLayout{};
	};
}
