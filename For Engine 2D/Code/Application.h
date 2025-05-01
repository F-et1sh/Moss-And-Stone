#pragma once
#include <forpch.h>

#include "Window.h"
#include "RenderContext.h"
#include "SceneHierarchyPanel.h"
#include "ContentBrowser.h"
#include "Framebuffer.h"
#include "MousePicker.h"

#include "SceneManager.h"

namespace FE2D {
	class FOR_API Application {
	public:
		Application() = default;
		~Application() = default;
	public:
		void Release();
		void Initialize(
			const vec2& window_resolution = vec2(800, 600),
			const std::string_view& window_name = "For Default Application",
			int monitor = -1);
		void Loop();

	public:
		inline void StartGameSession()noexcept { m_SceneManager.StartGameSession(); }
		inline void EndGameSession()noexcept { m_SceneManager.EndGameSession(); }

	private:
		RenderContext m_RenderContext;
		Window m_Window;

	private:
		SceneManager m_SceneManager;
		ResourceManager m_ResourceManager;

		friend class EditorApplication;
	};
}