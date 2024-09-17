#include "Pipeline.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace ili
{
	Pipeline::Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath,
		const PipelineConfigInfo& info) : m_Device(device)
	{
		CreateGraphicsPipeline(vertFilepath, fragFilepath, info);
	}

	std::vector<char> Pipeline::ReadFile(const std::string& filepath)
	{
		// The ate flag means that we are starting at the end of the file
		// The binary flag just means that we are reading the file in binary mode
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		// Tellg gets the position of the current character in the input stream
		// This is the size of the file because we are starting at the end
		const size_t fileSize = (size_t)file.tellg();

		std::vector<char> buffer(fileSize);

		// Seekg moves the position of the current character in the input stream
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void Pipeline::CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& info)
	{
		std::string test = std::filesystem::current_path().string();

		const auto vertCode = ReadFile(vertFilepath);
		const auto fragCode = ReadFile(fragFilepath);

		// print the current path of the executable

		std::cout << "vertex shader code size: " << vertCode.size() << "\n";
		std::cout << "fragment shader code size: " << fragCode.size() << "\n";
	}

	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}
	}

	PipelineConfigInfo Pipeline::DefaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo configInfo{};

		return configInfo;
	}
}
