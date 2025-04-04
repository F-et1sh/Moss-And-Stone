#pragma once
#include <forpch.h>

#include "Window.h"

namespace FE2D {
	class FOR_API Application {
	public:
		Application() = default;
		~Application() = default;
	public:
		void Release(){}
		void Initialize(
			const vec2& window_resolution = vec2(800, 600),
			const std::string_view& window_name = "For Default Application",
			const size_t& monitor = -1){}
		void Loop();

	private:
		Window m_Window;

	private:
		ResourceManager m_ResourceManager;
		Scene m_Scene;
	};
}