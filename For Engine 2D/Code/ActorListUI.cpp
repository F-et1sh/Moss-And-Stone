#include "forpch.h"
#include "ActorListUI.h"

#include "Component.h"
#include "InspectorUI.h"

#include "UMapComponent.h"

void FE2D::ActorListUI::Release() {
    m_ActorList = nullptr;
    m_InspectorUI = nullptr;
}

void FE2D::ActorListUI::Initialize(std::vector<AActor*>* load_actorList, InspectorUI* load_inspectorUI) noexcept {
    if (!load_actorList)
        FOR_RUNTIME_ERROR("Failed to Initialize ActorListUI\nActorList was nullptr");
    if (!load_inspectorUI)
        FOR_RUNTIME_ERROR("Failed to Initialize ActorListUI\nInspectorUI was nullptr");

    m_ActorList = load_actorList;
    m_InspectorUI = load_inspectorUI;
}

void FE2D::ActorListUI::Update() {
    ImGui::Begin("Actor List");

    // is cursor hovered any of actors in the list
    bool isItemHovered = false;

    for (AActor* actor : *m_ActorList) {
        bool isSelected = false;

        if (ImGui::Selectable(actor->m_Name.empty() ? " " : actor->m_Name.c_str(), &isSelected)) {
            m_SelectedActor = actor;
            m_SelectedActor->OnSelected();
            m_InspectorUI->ChangeSelectedItem(m_SelectedActor);
        }

        if (ImGui::IsItemHovered())
            isItemHovered = true;

        constexpr float offset_from_actor = 250.0f;

        ImGui::SameLine(offset_from_actor);
        ImGui::Text(actor->m_IsVisible ? "Visible" : "Invisible");
    }

    if (m_SelectedActor) {
        this->processSelected();
        this->actorPopup(isItemHovered);
    }
    if (!isItemHovered)
        this->windowPopup();

    ImGui::End();
}

void FE2D::ActorListUI::processSelected() {
    if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        m_SelectedActor->Release();
        m_ActorList->erase(std::find(m_ActorList->begin(), m_ActorList->end(), m_SelectedActor));
        m_SelectedActor = nullptr;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_F2)) {
        m_IsRenaming = true;
    }

    if (m_IsRenaming) {
        char buffer[256];
        strncpy_s(buffer, m_SelectedActor->m_Name.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText("##Rename", buffer, sizeof(buffer))) {
            m_SelectedActor->m_Name = buffer[0] ? buffer : " ";  // actor's name mustn't be empty
        }

        if (ImGui::IsItemDeactivatedAfterEdit())
            m_IsRenaming = false;
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        m_SelectedActor->OnDeselected();
        m_SelectedActor = nullptr;
        m_InspectorUI->ChangeSelectedItem(m_SelectedActor);
    }
}

void FE2D::ActorListUI::actorPopup(bool isItemHovered) {
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isItemHovered)
        ImGui::OpenPopup("ActorPopup");
    
    if (ImGui::BeginPopup("ActorPopup")) {
        if (ImGui::MenuItem("Rename")) {
            m_IsRenaming = true;
        }
        if (ImGui::MenuItem("Delete")) {
            m_SelectedActor->Release();
            m_ActorList->erase(std::find(m_ActorList->begin(), m_ActorList->end(), m_SelectedActor));
            m_SelectedActor = nullptr;
        }
        ImGui::EndPopup();
    }
}

void FE2D::ActorListUI::windowPopup() {
    if (!ImGui::BeginPopupContextWindow())
        return;

    if (ImGui::MenuItem("Add Actor")) {
        m_ActorList->push_back(new AActor("New Actor " + std::to_string(m_ActorList->size())));
    }

    if (ImGui::MenuItem("Add Camera")) {
        m_ActorList->push_back(new AActor("New Camera " + std::to_string(m_ActorList->size())));
        m_ActorList->back()->addComponent<UCameraComponent>();
    }

    if (ImGui::MenuItem("Add Sprite")) {
        m_ActorList->push_back(new AActor("New Sprite " + std::to_string(m_ActorList->size())));
        m_ActorList->back()->addComponent<USpriteComponent>();
    }
    }

    ImGui::EndPopup();
}
