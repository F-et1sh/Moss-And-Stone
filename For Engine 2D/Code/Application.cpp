#include "forpch.h"
#include "Application.h"

void FE2D::Application::Loop() {
	while (m_Window.isOpen()) {
		m_Window.Update_DeltaTime();

		m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Window.SwapBuffers();
		m_Window.PollEvent();
	}
}