#pragma once
#include <forpch.h>

#include "RenderContext.h"
#include "SceneHierarchyPanel.h"
#include "ContentBrowser.h"
#include "Framebuffer.h"
#include "MousePicker.h"
#include "Cursor.h"

#include "ProjectVariables.h"
#include "SceneManager.h"

namespace FE2D {
	class FOR_API Application {
	public:
		Application() = default;
		~Application();
	
		void Initialize(
			const vec2& window_resolution = vec2(800, 600),
			const std::string_view& window_name = "For Default Application",
			int monitor = -1);
		void Loop();

	private:
		Window m_Window;
		ResourceManager m_ResourceManager;
		SceneManager m_SceneManager;
		
		RenderContext m_RenderContext;

		ProjectVariables m_ProjectVariables;

		friend class EditorApplication;
	};
}