#include "forpch.h"
#include "EditorApplication.h"

FE2D::EditorApplication::~EditorApplication() {
	m_ImGui.Release();
	GLFW::Release();
}

void FE2D::EditorApplication::Initialize(const vec2& window_resolution, const std::string_view& window_name, int monitor) {
	GLFW::Initialize();

	m_ProjectVariables.Load();

	m_Window.Initialize(window_resolution, window_name.data(), monitor);
	m_RenderContext.Initialize();
	m_RenderContext.BindCamera(m_EditorCamera);

	m_Window.setVSyn(true);
	m_Window.setAutoClose(false);

	m_Window.SubscribeToEvent(m_Event_WindowClosed, EventType::WindowClosed, [&](const IEvent& e) { this->make_close_request(); });
	m_Window.SubscribeToEvent(m_Event_WindowFocus, EventType::WindowFocus, [&](const IEvent& e) {
		// Reinitialize ImGui after window gained
		try {
			m_ImGui.Initialize(m_Window, m_RenderContext, m_ResourceManager, m_ProjectVariables);
		}
		catch (const std::exception& e) {
			SAY("Failed to Reinitilize ImGui after window focus\n" << e.what());
		}
		}
	);

	m_Window.SubscribeToEvent(m_Event_KeyPressed, EventType::KeyPressed, [&](const IEvent& e) {
		const KeyPressed& event = *static_cast<const KeyPressed*>(&e);
		// CTRL + S
		if (event.ctrl && event.key == GLFW_KEY_S) this->Save();
		});

	m_Window.SubscribeToEvent(m_Event_MouseWheelScrolled, EventType::MouseWheelScrolled, [&](const IEvent& e) {
		if (!m_IsPreviewHovered) return;

		const MouseWheelScrolled* wheel = static_cast<const MouseWheelScrolled*>(&e);
		
		constexpr float scroll_speed = 0.1f;
		float zoom_factor = 1.0f - (wheel->offset.y * scroll_speed);
		zoom_factor = 1.0f / zoom_factor;

		m_EditorCamera.setZoom(m_EditorCamera.getZoom() * zoom_factor);
		});

	m_ImGui.Initialize(m_Window, m_RenderContext, m_ResourceManager, m_ProjectVariables);

	m_ResourceManager.Initialize();

	m_SceneManager.Initialize(m_Window, m_RenderContext, m_ResourceManager, m_ProjectVariables);
	
	m_SceneHierarchyPanel.setContext(m_SceneManager.getCurrentScene(), m_ImGui, m_MousePicker);
	m_ContentBrowser.Initialize(m_Window, m_ResourceManager, m_ImGui);

	const vec2 game_resolution = vec2(m_RenderContext.getViewport().z, m_RenderContext.getViewport().w);
	m_GameFramebuffer.Initialize(game_resolution);
	m_MousePicker.Initialize(game_resolution);
	
	m_IsRunning = true;
}

void FE2D::EditorApplication::Loop() {
	while (m_IsRunning) {
		m_Window.Update_DeltaTime();

		m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->UpdateCameraMoving();

		m_ImGui.BeginFrame();

		this->OnGameUpdate();

		if (m_IsPreviewHovered) this->OnPickerUpdate();

		this->OnImGuiRender();

		m_ImGui.EndFrame();

		m_Window.SwapBuffers();
		m_Window.PollEvent();
	}
}

void FE2D::EditorApplication::OnMainMenuBar() {
	if (!ImGui::BeginMainMenuBar()) return;

	if (ImGui::BeginMenu("File")) {

		if (ImGui::MenuItem("Save", "Ctrl+S")) {
			this->Save();
		}

		if (ImGui::MenuItem("Exit")) {
			make_close_request();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Scene")) {
		
		if (ImGui::MenuItem("Physics Layers")) {
			m_IsLayerWindowOpen = true;
		}

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void FE2D::EditorApplication::OnCloseRequest() {
	ImGui::OpenPopup("Save Changes?");

	if (ImGui::BeginPopupModal("Save Changes?", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
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

		if (ImGui::Button("Cancel") || ImGui::IsKeyDown(ImGuiKey_Escape)) {
			m_CloseRequest = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void FE2D::EditorApplication::Save() {
	m_ProjectVariables.Save();
	m_ResourceManager.save_resources();
	m_SceneManager.SaveCurrentScene();
}

void FE2D::EditorApplication::OnGameUpdate() {
	m_GameFramebuffer.Bind();

	m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
	m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_SceneManager.Update();

	m_GameFramebuffer.Unbind();
}

void FE2D::EditorApplication::OnPickerUpdate() {
	m_MousePicker.Bind();

	m_Window.ClearScreen(GL_DEPTH_BUFFER_BIT);

	int background[] = { -1, -1, -1, -1 };
	glClearBufferiv(GL_COLOR, 0, background);
	
	m_SceneManager.RenderPickable(m_RenderContext, m_MousePicker);
		
	m_MousePicker.Unbind();
}

void FE2D::EditorApplication::OnImGuiRender() {

	if (m_CloseRequest)					  this->OnCloseRequest();
	if (m_ContentBrowser.m_DeleteRequest) this->m_ContentBrowser.OnDeleteRequest();

	m_ImGui.StartDockSpace();

	this->OnMainMenuBar();

	this->OnPreviewWindow();

	m_SceneHierarchyPanel.OnImGuiRender(m_IsPreviewHovered, m_PreviewMousePosition, m_SceneManager.IsGameSessionRunning());
	m_ContentBrowser.OnImGuiRender();

	auto& physics_layers = m_ProjectVariables.getPhysicsLayers();
	auto& layers = physics_layers.getLayers();

	if (m_IsLayerWindowOpen) {
		if (ImGui::Begin("Layers", &m_IsLayerWindowOpen)) {

			constexpr static int count = physics_layers.FOR_PHYSICS_LAYERS_COUNT;

			std::vector<int> named;
			for (int i = 0; i < count; i++)
				if (!layers[i].name.empty())
					named.push_back(i);

			if (ImGui::BeginTable("CollisionTable", named.size() + 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				for (int col : named) {
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(layers[col].name.c_str());
				}
				ImGui::TableNextColumn();

				int layer_to_delete = -1;

				for (int row : named) {
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(layers[row].name.c_str());

					for (int col : named) {
						ImGui::TableNextColumn();
						if (col > row) continue;

						bool on = physics_layers.canCollide(row, col);
						std::string id = "##l" + std::to_string(row) + '_' + std::to_string(col);
						if (ImGui::Checkbox(id.c_str(), &on))
							physics_layers.setCollision(row, col, on);
					}

					ImGui::TableNextColumn();
					std::string del_id = "remove##del" + std::to_string(row);
					if (ImGui::SmallButton(del_id.c_str()))
						layer_to_delete = row;
				}
				ImGui::EndTable();

				if (layer_to_delete >= 0)
					physics_layers.removeLayer(layer_to_delete);
			}

			static std::string new_name = "NewLayer";

			m_ImGui.InputText("Layer Name", new_name);
			ImGui::SameLine();
			if (ImGui::Button("Add Layer"))
				physics_layers.createLayer(new_name);
		}
		ImGui::End();
	}
}

void FE2D::EditorApplication::OnPreviewWindow() {
	ImGui::Begin("Preview", nullptr, m_ImGui.IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

	ImGui::SameLine(ImGui::GetWindowWidth() / 2, 0); // draw the button in the middle of the window
	
	bool running = this->m_SceneManager.IsGameSessionRunning();
	std::string title = running ? "Stop" : "Start";

	if (ImGui::Button(title.c_str())) {
		if (running) { 
			this->m_SceneManager.EndGameSession();
			m_RenderContext.BindCamera(m_EditorCamera);
		}
		else {
			this->m_SceneManager.StartGameSession();
			m_SceneHierarchyPanel.resetSelected();
		}
	}

	const float aspect = (float)m_GameFramebuffer.get_texture_size().y / m_GameFramebuffer.get_texture_size().x;
	float width = ImGui::GetWindowWidth();
	float height = width * aspect;

	m_IsPreviewHovered = ImGui::IsWindowHovered();
	m_IsPreviewFocused = ImGui::IsWindowFocused();

	// to move the image down
	constexpr float offset_y = 0.0f;

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ((width - height) / 2) - offset_y));

	m_PreviewMousePosition.x = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x;
	m_PreviewMousePosition.y = height - ((ImGui::GetWindowHeight() - ImGui::GetCursorScreenPos().y) - (ImGui::GetWindowHeight() - ImGui::GetMousePos().y));

	m_PreviewMousePosition /= vec2(width, height);
	m_PreviewMousePosition *= LOGICAL_RESOLUTION;

	m_ImGui.PreviewWindowPosition(vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));
	m_ImGui.PreviewWindowSize(vec2(width, height));
	m_ImGui.PreviewImagePosition(vec2(ImGui::GetCursorScreenPos().x, (ImGui::GetIO().DisplaySize.y / 2) - ImGui::GetCursorScreenPos().y + 1)); // add 1 to the Y-axis to resolve a visual bug
	
	ImGui::Image(m_GameFramebuffer.texture_reference(), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB_DRAGGING")) {
			ResourceID<Prefab> dropped_prefab_id = *static_cast<ResourceID<Prefab>*>(payload->Data);
			
			Scene& scene = m_SceneManager.getCurrentScene();
			auto& prefab = m_ResourceManager.GetResource(dropped_prefab_id);
			prefab.CreateEntity(scene);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

void FE2D::EditorApplication::UpdateCameraMoving() {
	if (!m_IsPreviewFocused) return;

	vec2 direction = vec2(0.0f);

	const float move_speed = 666 * m_Window.getDeltaTime() / m_EditorCamera.getZoom();

	if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) direction.x += 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_LeftArrow )) direction.x -= 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_UpArrow   )) direction.y += 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_DownArrow )) direction.y -= 1.0f;

	if (glm::length(direction) > 0.0f)
		direction = normalize(direction);

	m_EditorCamera.setPosition(m_EditorCamera.getPosition() + (direction * move_speed));
}
