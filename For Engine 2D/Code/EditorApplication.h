#pragma once
#include "Application.h"

#include "ActorListUI.h"
#include "ResourceUI.h"
#include "ProfileWindowUI.h"

#include "Framebuffer.h"

namespace FE2D {
	class FOR_API EditorApplication : public Application {
	public:
		EditorApplication() = default;
		~EditorApplication() = default;

		void Release()override;
		void Initialize(
			const vec2& window_resolution = vec2(800, 600),
			const std::string_view& window_name = "For Default Application",
			const size_t& monitor = -1)override;
		void Loop()override;

	private:
		void camera_setting();

		void process_game();
		void draw_Interface();
		void draw_GameView();
	private:
		ResourceUI m_ResourceUI = ResourceUI(this->getResourceManager()); // set the ResourceManager
		InspectorUI m_InspectorUI;
		ActorListUI m_ActorListUI;
		ProfileWindowUI m_ProfileWindowUI;

		Camera m_CameraUI;
	private:
		Framebuffer m_GameView;

		bool m_IsViewHovered = false;
		bool m_IsMousePressed = false;

		vec2 m_Offset = vec2();
		vec2 m_MousePosition = vec2();

		vec2 m_GameView_WindowSize = vec2();

		bool m_IsGameRunning = false;
	private:
		void close_request();
		bool m_IsRunning = true;
		bool m_MakeCloseRequest = false;
	};
}