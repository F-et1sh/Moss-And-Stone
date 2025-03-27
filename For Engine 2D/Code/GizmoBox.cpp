#include "forpch.h"
#include "GizmoBox.h"

void FE2D::GizmoBox::Release() {
	m_ButtonManager.Release();
	m_OutlineMesh.Release();
}

void FE2D::GizmoBox::Initialize(Window* window, Camera* camera) {
	m_ButtonManager.Initialize(window, camera);

	constexpr vec2 vertices[] = {
		vec2(0.0f - 0.5f, 1.0f - 0.5f),
		vec2(1.0f - 0.5f, 1.0f - 0.5f),
		vec2(1.0f - 0.5f, 0.0f - 0.5f),
		vec2(0.0f - 0.5f, 0.0f - 0.5f),
		vec2(0.0f - 0.5f, 1.0f - 0.5f)
	};
	m_OutlineMesh.Initialize(vertices, 0, 5, GL_STATIC_DRAW, GL_LINE_STRIP);
	m_OutlineMesh.m_Size = vec2(100, 100);
	m_OutlineMesh.m_Layer = 900.0f;
	
	constexpr vec2 buttonPositions[] = {
		vec2(-0.5f,  0.0f), // Left
		vec2( 0.5f,  0.0f),	// Right
		vec2( 0.0f,  0.5f),	// Top
		vec2( 0.0f, -0.5f)	// Bottom
	};

	for (const auto& pos : buttonPositions) {
		UI_Button* button = new UI_Button();
		button->Initialize();
		
		button->m_DynamicX = true;
		button->m_DynamicY = true;
		
		button->m_Position = pos * m_OutlineMesh.m_Size;
		
		button->m_Size_Normal  = vec2(10, 10);
		button->m_Size_Hitted  = vec2(10, 10);
		button->m_Size_Pressed = vec2( 7,  7);

		button->m_Size		   = vec2(10, 10);
		button->m_Mesh.m_Layer = 901.0f;

		m_ButtonManager.m_Buttons.push_back(button);
	}
}

void FE2D::GizmoBox::draw() {
	this->UpdateBox();

	m_OutlineMesh.draw();
	for (const auto& button : m_ButtonManager.m_Buttons)
		button->draw();
}

void FE2D::GizmoBox::UpdateBox() {
	// Find X Size of the Gizmo Box
	float size_x = m_ButtonManager.m_Buttons[Box::RIGHT]->m_Position.x-
				   m_ButtonManager.m_Buttons[Box::LEFT ]->m_Position.x;
	// Find Y Size of the Gizmo Box
	float size_y = m_ButtonManager.m_Buttons[Box::TOP   ]->m_Position.y-
				   m_ButtonManager.m_Buttons[Box::BOTTOM]->m_Position.y;

	m_OutlineMesh.m_Size.x = size_x; // Set the Size for Mesh
	m_OutlineMesh.m_Size.y = size_y; // Set the Size for Mesh

	// Move the Mesh by X
	m_OutlineMesh.m_Position.x = (m_ButtonManager.m_Buttons[Box::RIGHT]->m_Position.x+
								  m_ButtonManager.m_Buttons[Box::LEFT ]->m_Position.x)/2;
	// Move the Mesh by Y
	m_OutlineMesh.m_Position.y = (m_ButtonManager.m_Buttons[Box::TOP   ]->m_Position.y+
								  m_ButtonManager.m_Buttons[Box::BOTTOM]->m_Position.y)/2;

	// Set the Button's Muted Positions ( Y axis for Right and Left )
	m_ButtonManager.m_Buttons[Box::RIGHT]->m_Position.y = m_OutlineMesh.m_Position.y;
	m_ButtonManager.m_Buttons[Box::LEFT ]->m_Position.y = m_OutlineMesh.m_Position.y;

	// Set the Button's Muted Positions ( X axis for Top and Bottom )
	m_ButtonManager.m_Buttons[Box::TOP   ]->m_Position.x = m_OutlineMesh.m_Position.x;
	m_ButtonManager.m_Buttons[Box::BOTTOM]->m_Position.x = m_OutlineMesh.m_Position.x;

	// Set Side Positions
	{
		m_Left   = m_ButtonManager.m_Buttons[Box::LEFT  ]->m_Position.x;
		m_Right  = m_ButtonManager.m_Buttons[Box::RIGHT ]->m_Position.x;
		m_Top    = m_ButtonManager.m_Buttons[Box::TOP   ]->m_Position.y;
		m_Bottom = m_ButtonManager.m_Buttons[Box::BOTTOM]->m_Position.y;
	}
}
