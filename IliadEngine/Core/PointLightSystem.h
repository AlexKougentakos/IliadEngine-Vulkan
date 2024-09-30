#pragma once

#include "../Graphics/Pipeline.h"
#include "../Graphics/Device.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/GameObject.h"

namespace ili
{
	class PointLightGameObject;
	struct GlobalUbo;
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

		void Update(const FrameInfo& frameInfo, GlobalUbo& ubo, std::vector<std::unique_ptr<PointLightGameObject>>& pointLights);
		void Render(const FrameInfo& frameInfo, std::vector<std::unique_ptr<PointLightGameObject>>& pointLights);
	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;

		std::unique_ptr<Pipeline> m_Pipeline{};
		VkPipelineLayout m_PipelineLayout{};
	};
}
