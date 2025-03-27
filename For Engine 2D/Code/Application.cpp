#include "forpch.h"
#include "Application.h"

void FE2D::Application::Release() {
	m_SceneManager.Release();
	m_Input.Release();

	m_Window.Release();
	
	m_ResourceManager.Release();

	GLFW::Release();
}

void FE2D::Application::Initialize(const vec2& window_resolution, const std::string_view& window_name, const size_t& monitor) {
	GLFW::Initialize();

	m_Window.Initialize(window_resolution, window_name.data(), monitor);

	m_Input.Initialize(this->getWindow());

	m_ResourceManager.Initialize();

	m_SceneManager.Initialize(this->getWindow(), this->getInput(), this->getResourceManager());
}

void FE2D::Application::Loop() {
	while (m_Window.isOpen()) {
		m_Window.Update_DeltaTime();

		m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->m_SceneManager.Update();
		this->m_SceneManager.Render();

		m_Window.SwapBuffers();
		m_Window.PollEvent();
	}
}