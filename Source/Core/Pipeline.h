#pragma once
#include <string>
#include <vector>
#include "Device.h"

namespace ili
{

	struct PipelineConfigInfo
	{

	};

	class Pipeline
	{
	public:
		Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& info);
		~Pipeline() = default;

		Pipeline(const Pipeline& other) = delete;
		Pipeline(Pipeline&& other) noexcept = delete;
		Pipeline& operator=(const Pipeline& other) = delete;
		Pipeline& operator=(Pipeline&& other) noexcept = delete;

		static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);
	private:
		static std::vector<char> ReadFile(const std::string& filepath);

		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& info);
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		Device& m_Device;
		VkPipeline m_GraphicsPipeline{};
		VkShaderModule m_VertexShaderModule{};
		VkShaderModule m_FragmentShaderModule{};
	};
}
