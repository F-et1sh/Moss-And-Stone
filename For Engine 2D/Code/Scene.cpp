#include "forpch.h"
#include "Scene.h"

#include "Actor.h"

bool FE2D::Scene::SerializeToFolder(const std::filesystem::path& scene_folder_path) const {
    const std::filesystem::path scene_config = scene_folder_path / L"this_scene.fs";

    std::ofstream config_file(scene_config);

    if (!config_file.good()) {
        SAY("ERROR : Failed to serialize a scene. Failed open this_scene.fs file\nPath : " << scene_folder_path);
        return false;
    }

    json j;
    Serializable::save_value(m_SceneID, j, "Scene ID");
    
    config_file << j;
    config_file.close();

    for (const auto& actor : m_ActorList)
        actor->SerializeToFolder(scene_folder_path / actor->m_Name);

    return true;
}

bool FE2D::Scene::DeserializeFromFolder(const std::filesystem::path& scene_folder_path) {
    this->Release();

    const std::filesystem::path scene_config = scene_folder_path / L"this_scene.fs";

    std::ifstream config_file(scene_config);

    if (!config_file.good()) {
        SAY("ERROR : Failed to deserialize a scene. Failed open this_scene.fs file\nPath : " << scene_folder_path);
        return false;
    }

    json j;
    config_file >> j;
    config_file.close();
    
    Serializable::load_value(m_SceneID, j, "Scene ID");

    std::vector<std::filesystem::path> folders = ResourceManager::scan_folder(scene_folder_path, ResourceManager::ScanMode::FOLDERS);
    const size_t actors_count = folders.size();
    m_ActorList.reserve(actors_count);

    for (const auto& actor_folder : folders) {
        AActor* actor = new AActor();
        if (!actor->DeserializeFromFolder(actor_folder)) {
            SAY("WARNING : Failed to deserialize an actor\nPath : " << scene_folder_path);
            continue;
        }
        m_ActorList.emplace_back(actor);
    }

    return true;
}

void FE2D::Scene::Release() {
    this->End();

    m_ActorList.clear();

    m_Renderer.Release();
}

void FE2D::Scene::Initialize(ResourceManager* resourceManager, Window* window) {
    m_Renderer.Initialize(resourceManager);
    m_Camera.BindToWindow(window);

    m_Renderer.setCamera(&m_Camera);
}

void FE2D::Scene::End() {
    for (auto& actor : m_ActorList)
        actor->Release();
}

void FE2D::Scene::Start() {
    for (auto& actor : m_ActorList) {
        actor->Initialize();
        
        if (UCameraComponent* camera = actor->getComponent<UCameraComponent>()) {
            m_Camera.BindToCameraComponent(*camera);
        }
    }

    m_Renderer.setCamera(&m_Camera);
}

void FE2D::Scene::Update(double _deltaTime, Input& input) {
    for (auto& actor : m_ActorList) {
        actor->Update(_deltaTime, input);
    }
}

void FE2D::Scene::Render() {
    for (auto& actor : m_ActorList) {
        if (!actor->m_IsVisible)
            continue;

        m_Renderer.HandleActor(actor);
    }
    m_Renderer.Render();
}
