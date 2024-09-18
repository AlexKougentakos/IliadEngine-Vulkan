#pragma once
#include <string>
#include <vector>
#include "Device.h"

namespace ili
{

	struct PipelineConfigInfo
	{
		VkViewport viewport{};
		VkRect2D scissor{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		VkPipelineLayout pipelineLayout{};
		VkRenderPass renderPass{};
		uint32_t subpass{};
	};

	class Pipeline
	{
	public:
		Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& info);
		~Pipeline();

		Pipeline(const Pipeline& other) = delete;
		Pipeline(Pipeline&& other) noexcept = delete;
		Pipeline& operator=(const Pipeline& other) = delete;
		Pipeline& operator=(Pipeline&& other) noexcept = delete;

		static PipelineConfigInfo GetDefaultPipelineConfigInfo(uint32_t width, uint32_t height);

		void Bind(VkCommandBuffer commandBuffer);
	private:
		static std::vector<char> ReadFile(const std::string& filepath);

		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		Device& m_Device;
		VkPipeline m_GraphicsPipeline{};
		VkShaderModule m_VertexShaderModule{};
		VkShaderModule m_FragmentShaderModule{};
	};
}
