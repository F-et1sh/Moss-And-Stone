#include "forpch.h"
#include "EditorApplication.h"

void FE2D::EditorApplication::Release() {
	m_PickerShader.Release();
	m_MousePicker.Delete();

	m_GameFramebuffer.Delete();

	m_SceneManager.Release();

	m_Window.Release();
	m_ResourceManager.Release();

	m_ImGui.Release();
	GLFW::Release();
}

void FE2D::EditorApplication::Initialize(const vec2& window_resolution, const std::string_view& window_name, size_t monitor) {
	GLFW::Initialize();

	m_Window.Initialize(window_resolution, window_name.data(), monitor);

	m_Window.setVSyn(true);
	m_Window.setAutoClose(false);

	m_Window.SubscribeOnEvent(Event::EventType::WindowClosed, [&](const Event& e) { this->make_close_request(); });
	m_Window.SubscribeOnEvent(Event::EventType::WindowFocus, [&](const Event& e) {
		// Reinitialize ImGui after window gained
		try {
			m_ImGui.Initialize(&this->m_Window, &m_ResourceManager);
		}
		catch (const std::exception& e) {
			SAY("Failed to Reinitilize ImGui after window focus\n" << e.what());
		}
		}
	);

	m_ImGui.Initialize(&m_Window, &m_ResourceManager);

	m_ResourceManager.Initialize();

	m_IsRunning = true;

	m_SceneManager.Initialize(&m_Window, &m_ResourceManager);
	m_SceneHierarchyPanel.setContext(&m_SceneManager.getCurrentScene(), &m_ImGui, &m_MousePicker);

	m_EditorCamera.setVisionSize(m_Window.getResolution());
	m_SceneManager.setCamera(&m_EditorCamera);

	m_GameFramebuffer.Initialize(m_Window.getResolution());
	m_MousePicker.Initialize(m_Window.getResolution());

	m_PickerShader.Initialize(FOR_PATH.get_shaders_path() / L"Picker Default" / L"PickerDefault");
	m_PickerUBO.create();
	m_PickerUBO.bindBlock(1);
	m_PickerUBO.bufferData(256 * (64 + 16), nullptr, GL_DYNAMIC_DRAW);
}

void FE2D::EditorApplication::Loop() {
	while (m_IsRunning) {
		m_Window.Update_DeltaTime();

		m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		this->OnGameUpdate();

		if (m_IsPreviewHovered)
			this->OnPickerUpdate();

		this->OnImGuiRender();

		m_Window.SwapBuffers();
		m_Window.PollEvent();
	}
}

void FE2D::EditorApplication::OnMainMenuBar() {
	if (!ImGui::BeginMainMenuBar())
		return;

	if (ImGui::BeginMenu("File")) {

		if (ImGui::MenuItem("Save", "Ctrl+S")) {
			this->Save();
		}

		if (ImGui::MenuItem("Exit")) {
			make_close_request();
		}

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void FE2D::EditorApplication::close_request() {
	ImGui::OpenPopup("Save Changes?");

	if (ImGui::BeginPopupModal("Save Changes?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you want to save changes before exiting?");
		ImGui::Separator();

		if (ImGui::Button("Save")) {
			this->Save();
			m_IsRunning = false;
		}

		ImGui::SameLine();

		if (ImGui::Button("Don't Save")) {
			m_IsRunning = false;
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel")) {
			m_CloseRequest = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void FE2D::EditorApplication::Save() {
	m_ResourceManager.SaveResources();
	m_SceneManager.SaveCurrentScene();
}

void FE2D::EditorApplication::OnGameUpdate() {
	m_GameFramebuffer.Bind();

	m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
	m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_Window.getResolution().x, m_Window.getResolution().y);

	m_SceneManager.Update();

	m_GameFramebuffer.Unbind();
}

void FE2D::EditorApplication::OnPickerUpdate() {
	m_MousePicker.Bind();

	m_Window.ClearColor(vec4(0));
	m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const size_t width = m_PreviewWindowSize.x;
	const size_t height = m_PreviewWindowSize.x / (m_Window.getResolution().x / m_Window.getResolution().y);

	const size_t x = m_PreviewWindowPosition.x;
	const size_t y = m_PreviewWindowPosition.y + m_Window.getResolution().y - (m_PreviewWindowSize.y / 2) - (height / 2);

	glViewport(x, y, width, height);

	int zero = -1;
	glClearBufferiv(GL_COLOR, 0, &zero);
	
	m_SceneManager.RenderPickable(m_PickerShader, m_PickerUBO);
		
	m_MousePicker.Unbind();
}

void FE2D::EditorApplication::OnImGuiRender() {
	m_ImGui.BeginFrame();

	if (m_CloseRequest)
		this->close_request();

	m_ImGui.StartDockSpace();

	this->OnMainMenuBar();

	this->OnPreviewWindow();

	m_SceneHierarchyPanel.OnImGuiRender(m_IsPreviewHovered);

	m_ImGui.EndFrame();
}

void FE2D::EditorApplication::OnPreviewWindow() {
	ImGui::Begin("Preview");

	m_IsPreviewHovered = ImGui::IsWindowHovered();
	m_PreviewWindowPosition = vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	m_PreviewWindowSize = vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

	const size_t width = ImGui::GetWindowWidth();
	const size_t height = ImGui::GetWindowWidth() / (m_Window.getResolution().x / m_Window.getResolution().y);

	const size_t x = 0;
	const size_t y = m_Window.getResolution().y - (ImGui::GetWindowHeight() / 2) - (height / 2);

	m_ImGui.PreviewWindowViewport(x, y, width, height);
	m_ImGui.PreviewWindowPosition(m_PreviewWindowPosition);
	m_ImGui.PreviewWindowSize(m_PreviewWindowSize);

	m_ImGui.setCamera(m_SceneManager.getCamera());

	ImGui::SameLine(ImGui::GetWindowWidth() / 2, 0); // draw the button in the middle of this window

	if (ImGui::Button(m_IsGameRunning ? "Stop" : "Start")) {
		m_IsGameRunning ? m_SceneManager.EndGameSession(), m_SceneManager.setCamera(&m_EditorCamera)
						: m_SceneManager.StartGameSession();
		m_IsGameRunning = !m_IsGameRunning;
	}

	ImGui::Image(m_GameFramebuffer.texture_reference(), ImVec2(m_GameFramebuffer.get_texture_size().x, 
															   m_GameFramebuffer.get_texture_size().y), ImVec2(0, 1),
																									    ImVec2(1, 0));

	ImGui::End();
}