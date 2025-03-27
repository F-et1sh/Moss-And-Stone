#include "forpch.h"
#include "Actor.h"

bool FE2D::AActor::SerializeToFolder(const std::filesystem::path& actor_folder_path) const {
    if (!std::filesystem::exists(actor_folder_path))
        std::filesystem::create_directories(actor_folder_path);

    std::ofstream config_file(actor_folder_path / L"this_actor.fs");
    
    if (!config_file.good()) {
        SAY("WARNING : Failed to serialize an actor\nActor name : " << this->m_Name.c_str() << "\nPath : " << actor_folder_path);
        return false;
    }

    json j;
    Serializable::save_value(m_Name, j, "Name");
    config_file << j;

    config_file.close();

    for (const auto& [key, component] : m_Components) {
        const std::string_view component_name = ComponentFactory::Instance().getComponentName(typeid(*component).name());
        std::ofstream component_file((actor_folder_path / component_name).wstring() + L".fs");
        if (!component_file.good()) {
            SAY("WARNING : Failed to serialize a component" <<
                "\nName : " << component_name.data() <<
                "\nPath : " << (actor_folder_path / component_name));
            continue;
        }
        
        json component_j;
        component_j["Type"] = typeid(*component).name();
        component_j["Data"] = component->Serialize();

        component_file << component_j;

        component_file.close();
    }

    return true;
}

bool FE2D::AActor::DeserializeFromFolder(const std::filesystem::path& actor_folder_path) {
    this->Release();
    this->m_Components.clear();

    std::ifstream config_file(actor_folder_path / L"this_actor.fs");

    if (!config_file.good()) {
        SAY("WARNING : Failed to deserialize an actor\nPath : " << actor_folder_path);
        return false;
    }

    json j;
    config_file >> j;
    config_file.close();

    Serializable::load_value(m_Name, j, "Name");

    const std::vector<std::filesystem::path> files = ResourceManager::scan_folder(actor_folder_path, ResourceManager::ScanMode::FILES);

    m_Components.reserve(files.size());

    for (const auto& file : files) {
        if (file.filename() == L"this_actor.fs")
            continue; // the scanning is happends in the same, actor's, folder. Cause we need to skip the config file
        
        std::ifstream component_file(file);
        if (!component_file.good()) {
            SAY("WARNING : Failed to deserialize a component\nPath : " << file);
            continue;
        }

        json component_j;
        component_file >> component_j;

        const std::string type_name = component_j["Type"];
        UComponent* component = ComponentFactory::Instance().CreateComponent(type_name);
        try {
            component->setActor(this);
            component->Deserialize(component_j["Data"]);

            const size_t hash_code = typeid(*component).hash_code();

            m_Components.emplace_back(hash_code, std::unique_ptr<UComponent>(component));
        }
        catch (const std::exception& e) {
            SAY("WARNING : Failed to deserialize a component\nType name : " << type_name.c_str() << "\nException : " << e.what());
        }
        
        component_file.close();
    }

    return true;
}

void FE2D::AActor::Release() {
    for (auto& component : m_Components)
        component.second->Release();
}

void FE2D::AActor::Initialize() {
    for (auto& component : m_Components) {
        if (!component.second->Initialize())
            SAY("WARNING : Failed to Initialize Component\nComponent : " << typeid(*component.second).name());
    }
}

void FE2D::AActor::Update(double _deltaTime, Input& input) {
    for (auto& component : m_Components) {
        if (component.second->m_IsEnabled) {

            component.second->OnInput(input);
            component.second->Update(_deltaTime);
        }
    }
}