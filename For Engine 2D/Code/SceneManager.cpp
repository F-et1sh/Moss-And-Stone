#include "forpch.h"
#include "SceneManager.h"

void FE2D::SceneManager::Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager) {
	m_Window = &window;
	m_RenderContext = &render_context;
	m_ResourceManager = &resource_manager;

	LoadAvailableScenes();
	
	if (!LoadScene(START_SCENE)) {
		SAY("WARNING : Failed to load start scene. Loading the first loaded\nStart scene index : " << START_SCENE);

		if (m_ScenePaths.empty()) {
			SAY("WARNING : There is no loaded scene. Creating new scene");
			m_CurrentScene.Initialize(window, render_context, resource_manager);
		}
		else {
			// first loaded scene index
			SceneIndex scene_index = m_ScenePaths.begin()->first;
			LoadScene(scene_index);
		}
	}
}

void FE2D::SceneManager::Update() {
	if (m_IsRunning) m_CurrentScene.Update();
					 m_CurrentScene.Render();
}

void FE2D::SceneManager::SaveCurrentScene() {

	SceneSerializer ser(&m_CurrentScene);
	ser.Serialize(FOR_PATH.get_scenes_path() / (L"Scene_" + std::to_wstring(m_CurrentScene.getIndex()) + L".forscene"));
}

void FE2D::SceneManager::LoadAvailableScenes() {
	m_ScenePaths.clear();

	const auto& scenes_path = FOR_PATH.get_scenes_path();
	
	const auto& scenes = ResourceManager::scan_folder(scenes_path, ResourceManager::ScanMode::FILES);
	m_ScenePaths.reserve(scenes.size());

	for (const auto& scene_path : scenes) {

		if (scene_path.extension() != ".forscene")
			continue;

		std::ifstream file(scene_path);
		if (!file.good()) {
			SAY("WARNING : Failed to open scene file\nPath : " << scene_path);
			continue;
		}

		json j;
		try {
			file >> j;
		}
		catch (const std::exception& e) {
			SAY("WARNING : JSON parse error in scene file\nPath : " << scene_path << "\nReason : " << e.what());
			continue;
		}

		if (j.contains("SceneInfo")) {
			json j_sceneInfo = j["SceneInfo"];

			SceneIndex scene_index = 0;
			SceneSerializer::load_value(scene_index, j_sceneInfo, "SceneIndex");

			const auto& it = m_ScenePaths.find(scene_index);
			if (it != m_ScenePaths.end()) {
				SAY("WARNING : Duplicate scene index. Using last loaded\nScene Index" << scene_index << "\nPath 1 : " << it->second << "\nPath 2 : " << scene_path);
			}

			m_ScenePaths.emplace(scene_index, scene_path);
		}
		else {
			SAY("WARNING : Failed to read scene info\nPath : " << scene_path);
		}
	}
}

bool FE2D::SceneManager::LoadScene(SceneIndex index) {
	const auto& it = m_ScenePaths.find(index);

	if (it == m_ScenePaths.end()) {
		SAY("ERROR : Failed to load a scene. Bad index\nIndex : " << index);
		return false;
	}

	SceneIndex backup_index = m_CurrentScene.getIndex();
	//m_CurrentScene.~Scene();
	m_CurrentScene.Initialize(*m_Window, *m_RenderContext, *m_ResourceManager);

	SceneSerializer ser(&m_CurrentScene);
	if (!ser.Deserialize(it->second)) {
		SAY("ERROR : Failed to deserialize a scene\nIndex : " << index);
		
		this->LoadScene(backup_index);
		
		return false;
	}

	return true;
}
