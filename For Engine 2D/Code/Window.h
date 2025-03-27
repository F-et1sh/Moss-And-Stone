#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Event.h"
#include "EventTypes.h"
#include "EventDispatcher.h"

// Power of V-Syn While Game is not Maximized
#define FOR_LOW_OPERATION_MODE 4

// Polygon Offset Factor
#define FOR_POLYGON_FACTOR 1
// Polygon Offset Units
#define FOR_POLYGON_UNITS 1

#define FOR_COLOR_BUFFER_BIT 0x00004000
#define FOR_DEPTH_BUFFER_BIT 0x00000100

namespace FE2D {
	class FOR_API Window {
	public:
		Window() = default;
		~Window() = default;

		Window(const vec2& window_size, const std::string& window_name, size_t monitor) {
			this->Initialize(window_size, window_name, monitor);
		}

		// Close and Delete the Window
		void Release();

		// Initialize OpenGL stuff and Create new Window
		void Initialize(const vec2& window_size, const std::string& window_name, size_t monitor);

#pragma region Screen Update
	public:
		// Is the Window Open
		inline bool isOpen()const noexcept;

	private:
		inline void ApplyCallbacks()noexcept;
	public:
		// Clear Color
		inline void ClearColor(vec4 color)const noexcept;
		// Clear Color
		inline void ClearColor(float r, float g, float b, float a)const noexcept;
		// Clear Screen
		inline void ClearScreen(unsigned int buffer_bit)const noexcept;
		// Swap Front and Back Buffers
		inline void SwapBuffers()const noexcept;
		// Process all of Events in the Queue and Update the Input
		inline void PollEvent();
#pragma endregion
		
#pragma region get/set
	public:
		inline const vec2& getResolution()const noexcept;
		inline const vec2& getAspectRatio()const noexcept;
		inline void FindResolution()noexcept;
		inline void FindResolution(const unsigned int load_monitorID)noexcept;
		inline void setMaximazed(const bool maximized)noexcept;

		inline void setIcon(const std::string& path);
		inline void setTitle(const std::string& name)const noexcept;
		inline void setVSyn(int vsyn)noexcept;

		inline GLFWwindow* reference()const noexcept { return m_Window; }
#pragma endregion

#pragma region FPS Counter
	private:
		double m_LastTime_FPS = 0;
		size_t m_NbFrames = 0;
		size_t m_Current_FPS = 0;

		// FPS Showing/Updating Speed | Start Number 4
		float m_FPS_Speed = 4;
	public:
		// Set FPS Showing/Updating Speed
		inline void setFPS_Speed(float speed) { m_FPS_Speed = speed; }
		
		// Get FPS Showing/Updating Speed
		inline float getFPS_Speed() { return m_FPS_Speed; }
	public:
		inline const size_t& getFPS()noexcept;
#pragma endregion

#pragma region DeltaTime Counter

	private:
		using _clock = std::chrono::high_resolution_clock;
		using _time  = std::chrono::duration<double>;

		_clock::time_point m_LastTime_DeltaTime = _clock::now();
		_time			   m_DeltaTime			= _time(0);
	public:
		// Use this only one time in the beginning of the frame
		inline void Update_DeltaTime()noexcept;
		inline double getDeltaTime()const noexcept { return m_DeltaTime.count(); }

#pragma endregion

#pragma region Event System

	private:
		EventDispatcher m_EventDispatcher;

	public:
		// This function will return you listener's index you added
		// With this index you can unsubscribe on an event
		// 
		// @param You can find event type in Event::EventType, and to use it - use the class with the same name
		//
		// @param Second parameter needs std::function<void(const Event&)>
		inline size_t SubscribeOnEvent(const Event::EventType& type, const EventDispatcher::Listener& listener) {
			return m_EventDispatcher.subscribe(type, listener);
		}

		inline void UnsubscribeOnEvent(const Event::EventType& type, const EventDispatcher::ListenerID& id) {
			m_EventDispatcher.unsubscribe(type, id);
		}
		
	private:
		size_t m_WindowClose_EventIndex = 0;
	public:
		void setAutoClose(bool auto_close);

#pragma endregion

	private:
		bool m_IsRunning = false;

	private:
		int m_CurrentMonitor = 0;

		vec2 m_Resolution = vec2();
		vec2 m_AspectRatio = vec2();
		
		int m_VSyn = 0;

		int m_RefreshRate = 0;

	private:
		GLFWwindow* m_Window = nullptr;
		GLFWmonitor** m_Monitors = nullptr;
	};
}