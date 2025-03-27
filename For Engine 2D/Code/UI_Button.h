#pragma once
#include "Button.h"
#include "Mesh2D.h"

namespace FE2D {
	// UI_Button is a class derived from Button
	// It will follow the cursor when clicked
	class FOR_API UI_Button : public Button {
	public:
		friend class ButtonManager;
	public:
		UI_Button() = default;
		~UI_Button() = default;

		void Release();
		void Initialize();

		inline void draw()override {
			// Equate Mesh Size and Position to Button Size and Position
			m_Mesh.m_Position = m_Position;
			m_Mesh.m_Size = m_Size;

			m_Mesh.draw();
		}

		inline void ColorSetting(vec4&& hitted, vec4&& normal, vec4&& pressed)noexcept {
			m_Color_Hitted  = hitted;
			m_Color_Normal  = normal;
			m_Color_Pressed = pressed;

			m_Mesh.m_Color = m_Color_Normal;
		}
		inline void ColorSetting(vec4&& all)noexcept {
			m_Color_Hitted  = all;
			m_Color_Normal  = all;
			m_Color_Pressed = all;

			m_Mesh.m_Color = m_Color_Normal;
		}

		inline void SizeSetting(vec2&& hitted, vec2&& normal, vec2&& pressed)noexcept {
			m_Size_Hitted  = hitted;
			m_Size_Normal  = normal;
			m_Size_Pressed = pressed;

			m_Size = m_Size_Normal;
		}
		inline void SizeSetting(vec2&& all)noexcept {
			m_Size_Hitted  = all;
			m_Size_Normal  = all;
			m_Size_Pressed = all;
			
			m_Size = m_Size_Normal;
		}
	public:
		vec4 m_Color_Hitted  = vec4(1.0f, 0.7f, 1.0f, 1.0f); // Color of the Button when Hitted by Cursor
		vec4 m_Color_Normal  = vec4(1.0f, 0.5f, 1.0f, 1.0f); // Default Color of the Button
		vec4 m_Color_Pressed = vec4(1.0f, 0.7f, 1.0f, 1.0f); // Color of the Button when Clocked

		vec2 m_Size_Hitted  = vec2(50.0f, 50.0f); // Size of the Button when Hitted by Cursor
		vec2 m_Size_Normal  = vec2(50.0f, 50.0f); // Default Size of the Button
		vec2 m_Size_Pressed = vec2(40.0f, 40.0f); // Size of the Button when Clocked
	public:
		Mesh2D m_Mesh;
	private:
		vec2 m_CursorPosition = vec2();
		vec2 m_Offset = vec2(); // Offset between Button Position and Cursor Position
	public:
		bool m_DynamicX = false; // The Button will follow cursor by this direction
		bool m_DynamicY = false; // The Button will follow cursor by this direction
	protected:
		void onMouseMoved(const vec2& cursor_position);
	};
}