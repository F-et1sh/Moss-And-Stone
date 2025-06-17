#include "forpch.h"
#include "UI_Button.h"

//void UI_Button::Release() {
//	m_Mesh.Release();
//}
//
//void UI_Button::Initialize() {
//	this->Release();
//
//	constexpr vec2 vertices[] = {
//		vec2(0.0f - 0.5f, 1.0f - 0.5f),
//		vec2(0.0f - 0.5f, 0.0f - 0.5f),
//		vec2(1.0f - 0.5f, 1.0f - 0.5f),
//		vec2(1.0f - 0.5f, 0.0f - 0.5f),
//	};
//	m_Mesh.Initialize(vertices, 0, 4, GL_STATIC_DRAW, GL_TRIANGLE_STRIP);
//
//	m_Mesh.m_Layer = 900.0f; // Set the Layer of the Mesh
//	m_Size = m_Size_Normal;  // Set Button Size
//
//	this->setIsHittedCallback([this](const bool hitted) {
//		if (hitted) {
//			m_Mesh.m_Color = m_Color_Hitted;
//			m_Size		   = m_Size_Hitted;
//		}
//		else { 
//			m_Mesh.m_Color = m_Color_Normal;
//			m_Size		   = m_Size_Normal;
//		}
//		});
//
//	this->setIsPressedCallback([this](const bool pressed) {
//		if (pressed) {
//			m_Mesh.m_Color = m_Color_Pressed;
//			m_Size		   = m_Size_Pressed;
//
//			// Set the Offset | Dragging has been stared
//			m_Offset = m_CursorPosition - m_Position;
//		}
//		else {
//			m_Mesh.m_Color = m_Color_Normal;
//			m_Size		   = m_Size_Normal;
//		}
//		});
//}
//
//void FE2D::UI_Button::onMouseMoved(const vec2& cursor_position) {
//	if (m_IsPressed) {
//		if (m_DynamicX) m_Position.x = cursor_position.x - m_Offset.x;
//		if (m_DynamicY) m_Position.y = cursor_position.y - m_Offset.y;
//	}
//	m_CursorPosition = cursor_position;
//}
