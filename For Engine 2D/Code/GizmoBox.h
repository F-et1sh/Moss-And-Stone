#pragma once

#include "ButtonManager.h"
#include "UI_Button.h"

//namespace FE2D {
//	class FOR_API GizmoBox {
//	public:
//		GizmoBox() = default;
//		~GizmoBox() { Release(); }
//	public:
//		void Release();
//		void Initialize(Window* window, Camera* camera);
//
//		void draw();
//	private:
//		void UpdateBox(); // Update the Mesh and Sides of Gizmo
//	private:
//		float m_Left   = 0.0f;
//		float m_Right  = 0.0f;
//		float m_Top    = 0.0f;
//		float m_Bottom = 0.0f;
//	public:
//		const float& get_left  ()const noexcept { return m_Left  ; }
//		const float& get_right ()const noexcept { return m_Right ; }
//		const float& get_top   ()const noexcept { return m_Top   ; }
//		const float& get_bottom()const noexcept { return m_Bottom; }
//
//		const vec2& get_size   ()const noexcept { return vec2(m_Right - m_Left, m_Top - m_Bottom); }
//	private:
//		enum Box {
//			LEFT, 
//			RIGHT,
//			TOP,
//			BOTTOM,
//		};
//		ButtonManager m_ButtonManager;
//	private:
//		Mesh2D m_OutlineMesh;
//	};
//}