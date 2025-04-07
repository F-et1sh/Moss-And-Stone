#include "forpch.h"
#include "EditorApplication.h"

void FE2D::EditorApplication::Release() {
	FOR_IMGUI.Release();
	GLFW::Release();
}

void FE2D::EditorApplication::Initialize(const vec2& window_resolution, const std::string_view& window_name, size_t monitor) {
	GLFW::Initialize();

	m_Window.Initialize(window_resolution, window_name.data(), monitor);

	m_Window.setVSyn(true);
	m_Window.setAutoClose(false);

	m_Window.SubscribeOnEvent(Event::EventType::WindowClosed, [&](const Event& e) { this->make_close_request(); });

	m_SceneHierarchyPanel.setContext(&m_Scene);

	FOR_IMGUI.Initialize(m_Window.reference());

	m_ResourceManager.Initialize();

	m_IsRunning = true;

	m_Scene.Initialize(&m_Window, &m_ResourceManager);

	SceneSerializer des(&m_Scene);
	des.Deserialize(FOR_PATH.get_assets_path() / "new_scene.fs");

	m_Scene.StartGame();
}

void FE2D::EditorApplication::Loop() {
	while (m_IsRunning) {

		m_Window.Update_DeltaTime();

		m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Scene.UpdateGame();
		m_Scene.RenderGame();

		this->OnImGuiRender();

		m_Window.SwapBuffers();
		m_Window.PollEvent();
	}

	SceneSerializer ser(&m_Scene);
	ser.Serialize(FOR_PATH.get_assets_path() / "new_scene.fs");
}

void FE2D::EditorApplication::OnMainMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
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
	// TODO : Save Resources and Serialize the Scene
}

void FE2D::EditorApplication::OnImGuiRender() {
	FOR_IMGUI.BeginFrame();

	if (m_CloseRequest)
		this->close_request();

	this->OnMainMenuBar();

	FOR_IMGUI.StartDockSpace();
	m_SceneHierarchyPanel.OnImGuiRender();

	FOR_IMGUI.EndFrame();
}
