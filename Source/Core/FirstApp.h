#pragma once
#include "Pipeline.h"
#include "Window.h"

namespace ili
{
	class FirstApp
	{
	public:
		FirstApp();
		~FirstApp();
		void Run();

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

	private:
		Window m_Window;
		Pipeline m_Pipeline{ "Assets/CompiledShaders/shader.vert.spv", "Assets/CompiledShaders/shader.frag.spv" };
	};
}
