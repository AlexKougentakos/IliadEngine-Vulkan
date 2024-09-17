#include "Pipeline.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace ili
{
	Pipeline::Pipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		CreateGraphicsPipeline(vertFilepath, fragFilepath);
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

	void Pipeline::CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		std::string test = std::filesystem::current_path().string();

		const auto vertCode = ReadFile(vertFilepath);
		const auto fragCode = ReadFile(fragFilepath);

		// print the current path of the executable

		std::cout << "vertex shader code size: " << vertCode.size() << "\n";
		std::cout << "fragment shader code size: " << fragCode.size() << "\n";
	}
}
