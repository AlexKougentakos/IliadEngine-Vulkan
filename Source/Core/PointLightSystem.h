#pragma once

#include "../Graphics/Pipeline.h"
#include "../Graphics/Device.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/GameObject.h"

namespace ili
{
	struct FrameInfo;

	class PointLightSystem
	{
	public:
		PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;
		PointLightSystem(PointLightSystem&&) = delete;
		PointLightSystem& operator=(PointLightSystem&&) = delete;

		void Render(const FrameInfo& frameInfo);
	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;

		std::unique_ptr<Pipeline> m_Pipeline{};
		VkPipelineLayout m_PipelineLayout{};
	};
}
