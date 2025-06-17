#pragma once

#include "Camera.h"

//namespace FE2D {
//	// Button is an area on the screen, without any Color or Texture
//	// To add a Texture you have to inherit this class
//	// 
//	// Button can't work without ButtonManager
//	// To use it you must create ButtonManager and then add this button there
//	// ButtonManager will not delete the Button on Release(), you have to do it yourself
//	class FOR_API Button {
//	public:
//		friend class ButtonManager;
//	public:
//		Button() = default;
//		~Button() = default;
//	public:
//		inline void setIsHittedCallback(std::function<void(const bool)> callback)noexcept;
//		inline void setIsPressedCallback(std::function<void(const bool)> callback)noexcept;
//		
//		inline const bool IsHitted()const noexcept { return m_IsHitted; }
//		inline const bool IsPressed()const noexcept { return m_IsPressed; }
//	public:
//		virtual void draw() = 0;
//	private:
//		int m_TriggerButton = GLFW_MOUSE_BUTTON_LEFT; // The Button will trigger on this Mouse Button
//	public:
//		inline void ChangeTriggerButton(int new_button) noexcept;
//	public:
//		vec2 m_Position = vec2();
//		vec2 m_Size = vec2(50, 50);
//	protected:
//		std::function<void(const bool)>  m_IsHitted_Callback; // The Cursor is Hitted  the Button
//		std::function<void(const bool)> m_IsPressed_Callback; // The Cursor is Pressed the Button
//	protected:
//		bool m_IsHitted  = false; // State to track if Cursor is over the Button
//		bool m_IsPressed = false; // State to track if Cursor is clicked the Button
//	};
//}
