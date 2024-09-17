#pragma once
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
	};
}
