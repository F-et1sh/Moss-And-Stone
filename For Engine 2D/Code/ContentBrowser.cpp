#include "forpch.h"
#include "ContentBrowser.h"

void FE2D::ContentBrowser::Initialize(Window& window, ResourceManager& resource_manager, IMGUI& imgui) {
    m_ResourceManager = &resource_manager;
    m_ImGui = &imgui;

    this->m_ResourceManager->load_available_resources();

    const std::filesystem::path base_path = FOR_PATH.get_assets_path() / L"FE2D" / L"ContentBrowser";
    this->LoadContentImage(m_DirectoryImage, base_path / L"folder_icon.png");
    this->LoadContentImage(m_EmptyImage    , base_path / L"empty_icon.png");
    this->LoadContentImage(m_AnimationImage, base_path / L"animation_icon.png");
    this->LoadContentImage(m_PrefabImage   , base_path / L"prefab_icon.png");

    m_RootDirectory = FOR_PATH.get_assets_path();
    m_CurrentDirectory = m_RootDirectory;
}

void FE2D::ContentBrowser::OnImGuiRender() {
    ImGui::Begin("Content Browser", nullptr, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

    if (m_CurrentDirectory != m_RootDirectory) {
        if (ImGui::Button("<--") || ImGui::IsMouseClicked(3)) // mouse 'back' button
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
    }

    try {
        int columnCount = int(ImGui::GetContentRegionAvail().x / m_ItemSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        this->OnEditorDraw();
        this->OnPanelDraw(); // draw the panel after drawing editor

        ImGui::Columns(1);
    }
    catch (const std::exception& e) {
        SAY(L"ERROR : Failed to update Content Browser panel" <<
            "\nCurrent Directory Path : " << m_CurrentDirectory <<
            "\nReason : " << e.what());
    }

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();

    ImGui::SetCursorScreenPos(windowPos);
    ImGui::InvisibleButton("DropTarget", size);
    ImGui::SetCursorScreenPos(cursorPos);

    this->OnEntityDrop(m_CurrentDirectory);

    ImGui::End();
}

void FE2D::ContentBrowser::OnPanelDraw() {
    for (auto& directory : std::filesystem::directory_iterator(m_CurrentDirectory)) {
        const auto& path = directory.path();
        const std::string filename_string = path.filename().string();
        const std::filesystem::path ext = path.extension();

        if (directory.is_directory()) { // folder drawing
            this->DrawDirectory(path);
            
            ImGui::TextWrapped(filename_string.c_str());
            ImGui::NextColumn();
        }
        else if (ResourceLoader::is_supported(ext)) { // file drawing
            this->DrawFile(path);
           
            ImGui::TextWrapped(filename_string.c_str());
            ImGui::NextColumn();
        }

        if (ImGui::IsWindowFocused() && ImGui::IsKeyDown(ImGuiKey_Delete) &&
            m_SelectedPath == path) {

            this->setDeletingPath(path);
        }
    }
}

#define TEXTURE_SIZE(TEXTURE) { m_ItemSize, m_ItemSize * (TEXTURE.getSize().y / TEXTURE.getSize().x) }

void FE2D::ContentBrowser::DrawDirectory(const std::filesystem::path& path) {
    const std::string filename_string = path.filename().string();

    ImVec4 color = DEFAULT_DIRECTORY_COLOR;
    if (m_SelectedPath == path)
        color = SELECTED_DIRECTORY_COLOR;

    // selectable image of the file
    ImGui::ImageButton(filename_string.c_str(),
        m_DirectoryImage.reference(),
        TEXTURE_SIZE(m_DirectoryImage),
        ImVec2(0, 1), ImVec2(1, 0), color);

    this->OnEntityDrop(path);

    // click to open the folder
    if (ImGui::IsItemHovered()) {
        
        if (ImGui::IsMouseDoubleClicked(0))
            m_CurrentDirectory = path;
        
        if (ImGui::IsMouseClicked(0))
            this->setSelected(nullptr, path);
    }
}

void FE2D::ContentBrowser::DrawFile(const std::filesystem::path& path) {
    const std::string filename_string = path.filename().string();

    const FE2D::UUID uuid = m_ResourceManager->GetResourceByPath(path);

    size_t texture_index = m_EmptyImage.reference();
    ImVec2 texture_size = TEXTURE_SIZE(m_EmptyImage);

    std::filesystem::path ext = path.extension();

    if (ResourceLoader::texture_supported_extensions.contains(ext)) {
        Texture& texture = m_ResourceManager->GetResource(ResourceID<Texture>(uuid));
        texture_index = texture.reference();
        texture_size = TEXTURE_SIZE(texture);
    }
    else if (ResourceLoader::animation_supported_extensions.contains(ext)) {
        texture_index = m_AnimationImage.reference();
        texture_size = TEXTURE_SIZE(m_AnimationImage);
    }
    else if (ResourceLoader::prefab_supported_extensions.contains(ext)) {
        texture_index = m_PrefabImage.reference();
        texture_size = TEXTURE_SIZE(m_PrefabImage);
    }

    ImVec4 color = DEFAULT_FILE_COLOR;
    if (m_SelectedPath == path) {
        color = SELECTED_FILE_COLOR;
    }

    // selectable image of the file
    ImGui::ImageButton(filename_string.c_str(),
        texture_index,
        texture_size, ImVec2(0, 1), ImVec2(1, 0), m_SelectedPath == path ? color : ImVec4(0, 0, 0, 0), color);

    if (uuid == 0) return;

    if (ResourceLoader::texture_supported_extensions.contains(ext)) {
        // click to open the folder
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseClicked(0)) {
                auto& resource = m_ResourceManager->GetResource(ResourceID<Texture>(uuid));
                this->setSelected(static_cast<IResource*>(&resource), path);
            }
            if (ImGui::IsMouseDoubleClicked(0)) {
                static const std::string aseprite_path = "C:\\Program Files\\TorAs\\aseprite.exe"; // hardcode
                std::string command = "(\"" + aseprite_path + "\" \"" + path.string() + "\")";
                std::system(command.c_str());
            }
        }
    }
    else if (ResourceLoader::prefab_supported_extensions.contains(ext)) {
        if (ImGui::BeginDragDropSource()) {
            auto& resource = m_ResourceManager->GetResource(ResourceID<Prefab>(uuid));
            if (!resource.HasComponent<TagComponent>()) {
                SAY("ERORR : Failed to drag prefab. Prefab hasn't TagComponent");
                return;
            }

            FE2D::UUID uuid = m_ResourceManager->GetResourceByPath(path);
            if (uuid == FE2D::UUID(0)) {
                SAY("ERROR : Failed to drag prefab. Prefab UUID is 0");
                return;
            }

			ResourceID<Prefab> prefab_id(uuid);

            auto& tag_component = resource.GetComponent<TagComponent>();

            ImGui::SetDragDropPayload("PREFAB_DRAGGING", &prefab_id, sizeof(ResourceID<Prefab>));
            ImGui::Text("%s", tag_component.tag.c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void FE2D::ContentBrowser::LoadContentImage(Texture& texture, const std::filesystem::path& path) {
    if (!texture.LoadFromFile(path)) {
        SAY("ERROR : Failed to load a content image in ContentBrowser" <<
            "\nPath : " << path);
    }
    std::ifstream file(path.wstring() + L".fs");
    if (file.good()) {
        json j;
        file >> j;

        texture.Deserialize(j);

        file.close();
    }
}

void FE2D::ContentBrowser::OnDeleteRequest() {
    ImGui::OpenPopup("Delete?");

    if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Do you really want to delete this directory?");
        ImGui::Separator();

        if (ImGui::Button("Delete")) {
            const std::filesystem::path& path = this->getDeletingPath();

            if (std::filesystem::is_directory(path)) {
                auto resource_paths = m_ResourceManager->scan_folder(path, ResourceManager::ScanMode::FILES);
                
                for (const auto& res_path : resource_paths) {
                    /*const auto& info = m_ResourceManager->getInfoByPath(res_path);

                    size_t type_hash_code = info.first;
                    size_t resource_index = info.second;

                    m_ResourceManager->removeResource(info.first, info.second);*/
                }
            }
            else {
                /*const auto& info = m_ResourceManager->getInfoByPath(path);

                size_t type_hash_code = info.first;
                size_t resource_index = info.second;

                m_ResourceManager->removeResource(info.first, info.second);*/
            }
            
            this->resetSelected();

            std::filesystem::remove_all(path);

            m_ResourceManager->load_available_resources();
            
            this->resetDeletingPath();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();

            this->resetDeletingPath();
        }

        ImGui::EndPopup();
    }
}

void FE2D::ContentBrowser::OnEntityDrop(const std::filesystem::path& path) {
    if (!ImGui::BeginDragDropTarget()) return;

    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAGGING")) {
        Entity dropped_entity = *static_cast<Entity*>(payload->Data);

        if (dropped_entity && dropped_entity.HasComponent<TagComponent>()) {
            std::string entity_name = dropped_entity.GetComponent<TagComponent>().tag + ".fp";

            const std::filesystem::path resource_path = FE2D::generate_unique_filename(path / entity_name);
            const std::filesystem::path metadata_path = resource_path.wstring() + L".fs";

            m_ResourceManager->getLoader().CreateResource<Prefab>(resource_path, dropped_entity);
            m_ResourceManager->getLoader().LoadMetadata(metadata_path);
        }
    }

    ImGui::EndDragDropTarget();
}

void FE2D::ContentBrowser::OnEditorDraw() {
    ImGui::Begin("Resource Editor", nullptr, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

    if (this->getSelected()) {
        const std::string filename = m_SelectedPath.filename().string();

        ImGui::PushID(filename.c_str());
        
        ImGui::Text(filename.c_str());
        m_Selected->OnEditorDraw(*m_ImGui);

        ImGui::PopID();
    }

    ImGui::End();
}
