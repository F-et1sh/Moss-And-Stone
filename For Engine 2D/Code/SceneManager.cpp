#include "forpch.h"
#include "SceneManager.h"

void FE2D::SceneManager::Release() {
    m_CurrentScene.Release();

    this->Remove_TMP_Scenes();
}

void FE2D::SceneManager::Initialize(Window* window, Input* input, ResourceManager* resource_manager) {
    if (!window)
        FOR_RUNTIME_ERROR("Failed to Initialize Scene Manager\nWindow was nullptr");
    m_Window = window;
    
    if (!input)
        FOR_RUNTIME_ERROR("Failed to Initialize Scene Manager\nInput was nullptr");
    m_Input = input;

    if (!resource_manager)
        FOR_RUNTIME_ERROR("Failed to Initialize Scene Manager\nResource Manager was nullptr");
    m_ResourceManager = resource_manager;

    // run though all of files of scenes folder and upload thier id to the catalog
    this->FindScenesInFolder();

    size_t default_scene_id = 0;

    // find default scene with id zero
    // if it's not found use first available scene
    // if no scenes found in the folder create new
    this->FindDefaultScene(default_scene_id);

    try {
        this->DeserializeScene(default_scene_id);
    }
    catch (const std::exception& e) {
        SAY("ERROR : Failed to deserialize default scene\nException : " << e.what());
    }

    this->m_CurrentScene.Initialize(m_ResourceManager, m_Window);
}

void FE2D::SceneManager::LoadScene(size_t scene_id) {
    this->SerializeCurrentScene();
    this->DeserializeScene(scene_id);

    this->m_CurrentScene.setEditorCamera(m_EditorCamera);
}

void FE2D::SceneManager::SerializeCurrentScene() {
    this->m_CurrentScene.End();
    this->TMP_DeserializeScene(m_LastSceneID);
        
    const size_t& past_scene_id = m_CurrentScene.getSceneID();
    const std::wstring& scene_filename = m_SceneCatalog[past_scene_id];

    std::filesystem::path scene_folder_path = FOR_PATH.get_scenes_path() / scene_filename;

    std::filesystem::remove_all(scene_folder_path);       // clear the folder at first
    std::filesystem::create_directory(scene_folder_path); // recreate it

    if (!m_CurrentScene.SerializeToFolder(scene_folder_path)) {
        SAY("ERROR : Failed to serialize current scene\nPath : " << scene_folder_path);
        return;
    }

    m_LastSceneID = past_scene_id;
}

void FE2D::SceneManager::DeserializeScene(size_t scene_id) {
    const std::wstring& scene_filename = m_SceneCatalog[scene_id];
    std::filesystem::path scene_folder_path = FOR_PATH.get_scenes_path() / scene_filename;

    if (!m_CurrentScene.DeserializeFromFolder(scene_folder_path))
        SAY("ERROR : Failed to deserialize current scene\nPath : " << scene_folder_path);
}

void FE2D::SceneManager::TMP_SerializeCurrentScene() {
    const size_t& past_scene_id = m_CurrentScene.getSceneID();
    const std::wstring& scene_filename = m_SceneCatalog[past_scene_id] + L".tmp";

    std::filesystem::path scene_folder_path = FOR_PATH.get_scenes_path() / scene_filename;

    std::filesystem::remove_all(scene_folder_path);       // clear the folder at first
    std::filesystem::create_directory(scene_folder_path); // recreate it

    if (!m_CurrentScene.SerializeToFolder(scene_folder_path)) {
        SAY("ERROR : Failed to serialize current scene\nPath : " << scene_folder_path);
        return;
    }

    m_LastSceneID = past_scene_id;
}

void FE2D::SceneManager::TMP_DeserializeScene(size_t scene_id) {
    const std::wstring& scene_filename = m_SceneCatalog[scene_id];
    std::filesystem::path scene_folder_path = FOR_PATH.get_scenes_path() / (scene_filename + L".tmp");

    if (!m_CurrentScene.DeserializeFromFolder(scene_folder_path))
        SAY("ERROR : Failed to deserialize current scene\nPath : " << scene_folder_path);
}

void FE2D::SceneManager::Remove_TMP_Scenes() {
    for (const auto& directory : std::filesystem::directory_iterator(FOR_PATH.get_scenes_path())) {
        try {
            if (directory.path().extension() == L".tmp")
                std::filesystem::remove_all(directory.path());
        }
        catch (const std::exception& e) {
            SAY(L"ERROR : Failed to delete a temporarily serialized scene\nPath : " << directory.path().wstring());
        }
    }
}

void FE2D::SceneManager::Update() {
    m_CurrentScene.Update(m_Window->getDeltaTime(), *m_Input);
}

void FE2D::SceneManager::Render() {
    m_CurrentScene.Render();
}

void FE2D::SceneManager::FindScenesInFolder() {
    try {
        for (const auto& directory : std::filesystem::directory_iterator(FOR_PATH.get_scenes_path())) {
            try {
                std::ifstream file(directory.path() / L"this_scene.fs");

                if (!file.good()) {
                    SAY(L"ERROR : Failed to load scene id\nFailed to open this_scene.fs file\nPath : " << directory.path());
                    continue;
                }

                json j;
                file >> j;

                size_t scene_id = 0;
                Serializable::load_value(scene_id, j, "Scene ID");

                if (m_SceneCatalog.find(scene_id) != m_SceneCatalog.end()) {
                    SAY(L"WARNING : Duplicate Scene ID : " << std::to_wstring(scene_id) +
                        L"\nPath : " << directory.path());
                    continue;
                }

                m_SceneCatalog.emplace(scene_id, directory.path().filename());
            }
            catch (const std::exception& e) {
                SAY(L"ERROR : Failed to load scene id to the catalog\nPath : " << directory.path() <<
                    L"\nException : " << FE2D::string_to_wstring(e.what()));
            }
        }
    }
    catch (const std::exception& e) {
        FOR_RUNTIME_ERROR("Failed to find scenes in the folder\n" + std::string(e.what()));
    }
}

void FE2D::SceneManager::FindDefaultScene(size_t& default_scene_id) {
    try {
        if (m_SceneCatalog.find(default_scene_id) == m_SceneCatalog.end()) {
            // if no scenes found
            if (!m_SceneCatalog.empty()) {
                SAY("WARNING : Default scene (ID 0) not found. Using first available scene as default");
                // change default scene id
                default_scene_id = m_SceneCatalog.begin()->first;
            }
            else {
                SAY("WARNING : No scenes available in the catalog. Creating new default scene");
                // write new scene to the catalog
                m_SceneCatalog[default_scene_id] = L"New Scene";
                const std::filesystem::path scene_path = FOR_PATH.get_scenes_path() / m_SceneCatalog[default_scene_id];
                // create new folder for the scene
                std::filesystem::create_directories(scene_path);
                // create file for new scene
                m_CurrentScene.SerializeToFolder(scene_path);
            }
        }
    }
    catch (const std::exception& e) {
        SAY("ERROR : Failed to find default scene\n" << e.what());
    }
}
