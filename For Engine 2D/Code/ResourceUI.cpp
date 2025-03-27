#include "forpch.h"
#include "ResourceUI.h"

#include "InspectorUI.h"

void FE2D::ResourceUI::Release() {
    m_FolderIconTexture.Release();
    m_FileIconTexture.Release();
}

void FE2D::ResourceUI::Initialize(Window* window, InspectorUI* load_inspectorUI) {
    if (!window)
        FOR_RUNTIME_ERROR("Failed to Initialize ResourceUI\nWindow was nullptr");

    if (!load_inspectorUI)
        FOR_RUNTIME_ERROR("Failed to Initialize ResourceUI\nInspectorUI was nullptr");
    m_InspectorUI = load_inspectorUI;

    this->m_ResourceManager->load_available_resources();

    window->SubscribeOnEvent(Event::EventType::KeyPressed, [this](const Event& e) {
        const KeyPressed* event = static_cast<const KeyPressed*>(&e);

        if (event->ctrl && event->key == GLFW_KEY_R) // CTRL + R
            m_ResourceManager->load_available_resources();
        });

    if (!m_FolderIconTexture.LoadFromFile(FOR_PATH.get_assets_path() / L"FE2D" / L"ResourceUI" / L"folder_icon.png")) {
        SAY("ERROR : Failed to load folder icon texture in ResourceUI");
    }
    if (!m_FileIconTexture.LoadFromFile(FOR_PATH.get_assets_path() / L"FE2D" / L"ResourceUI" / L"file_icon.png")) {
        SAY("ERROR : Failed to load file icon texture in ResourceUI");
    }
    
    m_FilesDirectory = FOR_PATH.get_assets_path();
    m_CurrentDirectory = m_FilesDirectory;
}

void FE2D::ResourceUI::Update() {
    ImGui::Begin("Resource Manager");

    if (m_CurrentDirectory != m_FilesDirectory)
        if (ImGui::Button("<--") || ImGui::IsMouseClicked(3)) // mouse 'back' button
            m_CurrentDirectory = m_CurrentDirectory.parent_path();

    try {
        int columnCount = int(ImGui::GetContentRegionAvail().x / m_ItemSize);
        if (columnCount < 1) 
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        this->ShowResources();

        ImGui::Columns(1);

        this->HandleDeleting();

        this->HandleCreating();

        this->CancelSelection();
    }
    catch (const std::exception& e) {
        SAY(L"ERROR : Failed to update Resource Manager Panel in ResourceUI\nCurrent Directory Path : " << m_CurrentDirectory.wstring());
        SAY(e.what());
    }

    ImGui::End();
}

void FE2D::ResourceUI::ShowResources() {
    for (auto& directory : std::filesystem::directory_iterator(m_CurrentDirectory)) {
        const auto& path = directory.path();
        const std::string filenameString = path.filename().string();

        ImVec4 color = ImVec4(0, 0, 0, 0);
        if (m_Selected) {
            //if (std::filesystem::path(m_Selected->getFilePath()).filename() == path.filename())
                //color = ImVec4(1, 1, 1, 0.2);

            //SAY(std::filesystem::path(m_Selected->getFilePath()).filename());
            //SAY(path.filename());
        }

        // selectable image of the file
        ImGui::ImageButton(filenameString.c_str(),
            directory.is_directory() ? ImTextureID(m_FolderIconTexture.reference()) : ImTextureID(m_FileIconTexture.reference()),
            { m_ItemSize, m_ItemSize }, ImVec2(0, 1), ImVec2(1, 0), color);

        // click to open the folder
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                if (directory.is_directory()) {
                    m_CurrentDirectory = path;
                }
                else { // it's a file
                    

                    //m_Selected = m_ResourceManager->find_resource_by_path(path);
                    //m_InspectorUI->ChangeSelectedItem(m_Selected);


                }
            }
        }

        ImGui::TextWrapped(filenameString.c_str());

        ImGui::NextColumn();
    }
}

void FE2D::ResourceUI::HandleDeleting() {
    if (m_Selected && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        try {
            //std::filesystem::remove_all(FOR_PATH.get_executable_path() / m_Selected->getFilePath());
            m_Selected = nullptr;
        }
        catch (const std::exception& e) {
            //SAY(L"ERROR : Failed to delete a file/folder\nPath : " << m_Selected->getFilePath() << L"\n" << FE2D::string_to_wstring(e.what()));
        }
    }
}

void FE2D::ResourceUI::HandleCreating() {
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("New Folder")) {
            auto newFolderPath = m_CurrentDirectory / L"New Folder";
            std::filesystem::create_directory(newFolderPath);
        }
        if (ImGui::MenuItem("New Scene")) {
            // TODO : creating new scene
        }
        ImGui::EndPopup();
    }
}

void FE2D::ResourceUI::CancelSelection() {
    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
        m_Selected = nullptr;
        m_InspectorUI->ChangeSelectedItem(m_Selected);
    }
}
