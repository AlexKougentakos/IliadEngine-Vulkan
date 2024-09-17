#pragma once
#include <string>
#include <vector>

namespace ili
{

	class Pipeline
	{
	public:
		Pipeline(const std::string& vertFilepath, const std::string& fragFilepath);
		~Pipeline() = default;
		Pipeline(const Pipeline& other) = delete;
		Pipeline(Pipeline&& other) noexcept = delete;
		Pipeline& operator=(const Pipeline& other) = delete;
		Pipeline& operator=(Pipeline&& other) noexcept = delete;

	private:
		static std::vector<char> ReadFile(const std::string& filepath);

		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
	};
}
