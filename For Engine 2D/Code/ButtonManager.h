#pragma once
#include "Button.h"
#include "UI_Button.h"

namespace FE2D {
	// This class works like Batch Rendering in OpenGL
	// It's batching buttons together reducing count of event listeners
	// It's also may be disabled for optimization
	// You should use one instance of this manager per one window with buttons
	class FOR_API ButtonManager {
	public:
		ButtonManager() = default;
		~ButtonManager() { Release(); }

		void Release();
		void Initialize(Window* window, Camera* camera);

	private:
		void onWindowResized      (const Event& e);
		void onMouseMoved         (const Event& e);
		void onMouseButtonPressed (const Event& e);
		void onMouseButtonReleased(const Event& e);

		// Indices to unsubscribe on this events in Release() function
		std::unordered_map<Event::EventType, EventDispatcher::ListenerID> m_EventIndices;

	public:
		// Use this to manage buttons you adding
		std::vector<Button*> m_Buttons;

	private:
		Window* m_WindowReference = nullptr;
		Camera* m_CameraReference = nullptr;
	public:
		vec2 m_WindowResolution = vec2();
		vec2 m_CursorPosition = vec2();
	public:
		// If you have a lot of Buttons your program will be slow
		// Using this flag you can enable/disable handling of buttons of this Button Manager
		// It's true from start
		bool m_IsActive = true;
	};
}