#include "forpch.h"
#include "EditorApplication.h"

void FE2D::EditorApplication::Release() {
	// start here, before Application::Release()

	m_GameView.Delete();

	m_ActorListUI.Release();
	m_InspectorUI.Release();
	m_ResourceUI.Release();

	FOR_IMGUI.Release();

	Application::Release();
}

void FE2D::EditorApplication::Initialize(const vec2& window_resolution, const std::string_view& window_name, const size_t& monitor) {
	Application::Initialize(window_resolution, window_name, monitor);

	// start here after Application::Initialize()

	m_Window.setVSyn(false);
	m_Window.setAutoClose(false);

	m_Window.SubscribeOnEvent(Event::EventType::WindowClosed, [&](const Event& e) {
		this->m_MakeCloseRequest = true;
		});

	m_Window.SubscribeOnEvent(Event::EventType::WindowResized, [&](const Event& e) {
		const WindowResized* _event = static_cast<const WindowResized*>(&e);

		this->m_GameView.Initialize(_event->size);
		m_CameraUI.m_VisionSize = _event->size;
		});

	m_Window.SubscribeOnEvent(Event::EventType::KeyPressed, [&](const Event& e) {
		const KeyPressed* _event = static_cast<const KeyPressed*>(&e);
		
		if (_event->ctrl && _event->key == GLFW_KEY_S) { // CTRL + S
			this->m_SceneManager.SerializeCurrentScene();
			this->getResourceManager()->SaveResources();
		}
		});

	FOR_IMGUI.Initialize(this->getWindow()->reference());
	FOR_IMGUI.setResourceManager(this->getResourceManager());

	m_ResourceUI .Initialize(this->getWindow()   , &m_InspectorUI);
	m_InspectorUI.Initialize(&m_ResourceUI       , &m_ActorListUI);
	m_ActorListUI.Initialize(this->getActorList(), &m_InspectorUI);

	this->camera_setting();

	m_GameView.Initialize(this->getWindow()->getResolution());
}

void FE2D::EditorApplication::Loop() {
	while (m_IsRunning) {
		m_Window.Update_DeltaTime();

		m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		process_game();
		draw_Interface();

		m_Window.SwapBuffers();
		m_Window.PollEvent();
	}
}

void FE2D::EditorApplication::camera_setting() {
	m_Window.SubscribeOnEvent(Event::EventType::MouseWheelScrolled, [&](const Event& e) {
		if (!m_IsViewHovered)
			return;
		
		const MouseWheelScrolled* _event = static_cast<const MouseWheelScrolled*>(&e);

		constexpr float scroll_speed = 0.25f;

		m_CameraUI.setVisionSize(m_CameraUI.getVisionSize() - vec2(_event->offset.y) * scroll_speed * m_CameraUI.m_VisionSize);
		});

	m_Window.SubscribeOnEvent(Event::EventType::MouseButtonPressed, [&](const Event& e) {
		const MouseButtonPressed* _event = static_cast<const MouseButtonPressed*>(&e);

		if (_event->button == GLFW_MOUSE_BUTTON_MIDDLE &&
			m_IsViewHovered) {

			m_IsMousePressed = true;

			m_Offset = m_MousePosition - m_CameraUI.getPosition();
		}
		});

	m_Window.SubscribeOnEvent(Event::EventType::MouseButtonReleased, [&](const Event& e) {
		const MouseButtonReleased* _event = static_cast<const MouseButtonReleased*>(&e);

		if (_event->button == GLFW_MOUSE_BUTTON_MIDDLE)
			m_IsMousePressed = false;
		});

	m_Window.SubscribeOnEvent(Event::EventType::MouseMoved, [&](const Event& e) {
		const MouseMoved* _event = static_cast<const MouseMoved*>(&e);

		if (m_IsMousePressed) {
			m_CameraUI.setPosition(m_MousePosition - m_Offset);
		}

		constexpr float move_speed = 0.001f;
		const float speed_setter = (m_CameraUI.getVisionSize().x * move_speed);

		m_MousePosition = vec2(_event->position.x - m_CameraUI.getVisionSize().x / 2,
							  -_event->position.y + m_CameraUI.getVisionSize().y / 2) * speed_setter;
		});

	m_CameraUI.BindToWindow(nullptr);
	m_CameraUI.m_VisionSize = this->getWindow()->getResolution();

	m_SceneManager.setEditorCamera(&m_CameraUI);
}

void FE2D::EditorApplication::process_game() {
	m_GameView.Bind();

	const size_t width  = m_GameView_WindowSize.x;
	const size_t height = m_GameView_WindowSize.x / (m_Window.getResolution().x / m_Window.getResolution().y);
	
	constexpr float offset = 0.3f; // to move the view with game up

	const size_t x = 0;
	const size_t y = m_Window.getResolution().y - (m_GameView_WindowSize.y / 2) - (height / (2+offset));
	
	glViewport(x, y, width, height);

	m_Window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
	m_Window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	if (m_IsGameRunning)
		m_SceneManager.Update();
	
	m_SceneManager.Render();

	m_GameView.Unbind();
}

void FE2D::EditorApplication::draw_Interface() {
	FOR_IMGUI.BeginFrame();
	FOR_IMGUI.StartDockSpace();

	this->m_ResourceUI .Update();
	this->m_InspectorUI.Update();
	this->m_ActorListUI.Update();

	if (m_MakeCloseRequest)
		this->close_request();

	this->draw_GameView();

	FOR_IMGUI.EndFrame();
}

void FE2D::EditorApplication::draw_GameView() {
	ImGui::Begin("Moss And Stone", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	constexpr static float button_width = 50;
	constexpr static float button_height = 25;

	ImGui::SameLine((ImGui::GetWindowWidth() / 2) - (button_width / 2), 0);

	const char* label = m_IsGameRunning ? "Stop" : "Start";

	if (ImGui::Button(label, ImVec2(button_width, button_height))) {
		m_IsGameRunning = !m_IsGameRunning;
		
		if (m_IsGameRunning) {
			m_SceneManager.TMP_SerializeCurrentScene(); // temporarily serialize current scene
			m_SceneManager.StartGameSession();
		}
		else {
			m_SceneManager.StopGameSession();
			m_SceneManager.TMP_DeserializeScene(m_SceneManager.LastSceneID());
			m_SceneManager.setEditorCamera(&m_CameraUI);
			
			m_InspectorUI.ChangeSelectedItem(nullptr);
		}
	}

	float camera_pos[2] = { m_CameraUI.getPosition().x, m_CameraUI.getPosition().y };
	ImGui::InputFloat2("Camera Position", camera_pos);
	m_CameraUI.setPosition(vec2(camera_pos[0], camera_pos[1]));

	ImGui::Image(
		m_GameView.texture_reference(),
		ImVec2(m_Window.getResolution().x,
			   m_Window.getResolution().y),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	m_GameView_WindowSize.x = ImGui::GetWindowSize().x;
	m_GameView_WindowSize.y = ImGui::GetWindowSize().y;

	m_IsViewHovered = ImGui::IsWindowHovered();

	ImGui::End();
}

void FE2D::EditorApplication::close_request() {
	ImGui::OpenPopup("Save Changes?");

	if (ImGui::BeginPopupModal("Save Changes?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you want to save changes before exiting?");
		ImGui::Separator();

		if (ImGui::Button("Save")) {

			m_ResourceManager.SaveResources();
			m_SceneManager.SerializeCurrentScene();

			m_IsRunning = false;
		}
		
		ImGui::SameLine();

		if (ImGui::Button("Don't Save")) {
			m_IsRunning = false;
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel")) {
			m_MakeCloseRequest = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}