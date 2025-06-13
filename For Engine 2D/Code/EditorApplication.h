#pragma once
#include "Application.h"

namespace FE2D {
	class FOR_API EditorApplication {
	public:
		EditorApplication() = default;
		~EditorApplication() = default;

		void Release();
		void Initialize(
			const vec2& window_resolution = vec2(800, 600),
			const std::string_view& window_name = "For Default Application",
			int monitor = -1);
		void Loop();

	private:
		Window m_Window;
		RenderContext m_RenderContext;
		
		bool m_IsRunning = false;

	private:
		IMGUI m_ImGui;

	private:
		bool m_CloseRequest = false;
		inline void make_close_request()noexcept { m_CloseRequest = true; }

		// this must be called only in OnImGuiRender() function
		// to make a close request use make_close_request()
		void OnCloseRequest();
	private:

		void Save();

	private:

		void OnGameUpdate();

		// FILE | TOOLS | ETC
		void OnMainMenuBar();
		// entity picking by mouse
		void OnPickerUpdate();
		// method there you can use ImGui calls
		void OnImGuiRender();
		
		// game preview window
		void OnPreviewWindow();

		// update editor camera moving
		void UpdateCameraMoving();

	private:
		Camera m_EditorCamera;

		Framebuffer m_GameFramebuffer;

	private:
		bool m_IsPreviewHovered = false;
		bool m_IsPreviewFocused = false;
		// mouse position in bounds of preview window
		vec2 m_PreviewMousePosition = vec2();

		MousePicker m_MousePicker;

	private:
		SceneManager m_SceneManager;
		SceneHierarchyPanel m_SceneHierarchyPanel;

		ResourceManager m_ResourceManager;
		ContentBrowser m_ContentBrowser;
	};
}