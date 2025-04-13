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
			size_t monitor = -1);
		void Loop();

	private:
		void OnMainMenuBar();

	private:
		Window m_Window;
		bool m_IsRunning = false;

	private:
		IMGUI m_ImGui;
		Camera m_EditorCamera;

	private:
		bool m_CloseRequest = false;
		inline void make_close_request()noexcept { m_CloseRequest = true; }

		// this must be called only in OnImGuiRender() function
		// to make a close request use make_close_request()
		void close_request();
	private:

		void Save();

	private:

		void OnGameUpdate();
		void OnPickerUpdate();
		void OnImGuiRender();
		
		void OnPreviewWindow();

	private:
		Framebuffer m_GameFramebuffer;
		bool m_IsGameRunning = false;

	private:
		bool m_IsPreviewHovered = false;
		vec2 m_PreviewWindowPosition = vec2();
		vec2 m_PreviewWindowSize = vec2();

		MousePicker m_MousePicker;
		Shader m_PickerShader;
		UniformBuffer m_PickerUBO;

	private:
		SceneManager m_SceneManager;
		ResourceManager m_ResourceManager;
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}