#include "forpch.h"
#include "SceneHierarchyPanel.h"

// This needed to disable a security warning on std::strncpy()
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

void FE2D::SceneHierarchyPanel::setContext(Scene* context, IMGUI* imgui, MousePicker* mouse_picker) {
	m_Context = context;
	m_SelectedEntity = {};
	m_ImGui = imgui;
	m_MousePicker = mouse_picker;
}

void FE2D::SceneHierarchyPanel::OnImGuiRender(bool is_preview_hovered) {
	ImGui::Begin("Scene Hierarchy");

	if (m_Context) {
		const auto& view = m_Context->getRegistry().view<IDComponent>();

		// this needed to draw entities after loop, where was used entt::registry::view()
		// because you can't modify entities in this loop
		std::vector<Entity> entities_to_draw;
		entities_to_draw.reserve(view.size());

		for (auto entity_handle : view) {
			Entity entity = { entity_handle, m_Context };

			if (entity.HasParent())
				continue;

			entities_to_draw.emplace_back(entity);
		}

		for (auto& entity : entities_to_draw)
			DrawEntityNode(entity);

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		// right-click on blank space
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("New Entity");

			ImGui::EndPopup();
		}
	}
	ImGui::End();

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_preview_hovered) {
		int entity_index = m_MousePicker->ReadPixel(vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y));

		if (entity_index != -1) 
			m_SelectedEntity = { (entt::entity)entity_index, m_Context };
	}

	ImGui::Begin("Properties");
	if (m_SelectedEntity)
		DrawComponents(m_SelectedEntity);
	ImGui::End();
}

void FE2D::SceneHierarchyPanel::DrawEntityNode(Entity entity) {
	auto& tag = entity.GetComponent<TagComponent>().tag;
	UUID uuid = entity.GetUUID();

	ImGui::PushID(uuid.get());

	ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());

	if (ImGui::IsItemClicked())
		m_SelectedEntity = entity;
	
	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("ENTITY_RELATIONSHIP", &entity, sizeof(Entity));
		ImGui::Text("%s", tag.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_RELATIONSHIP")) {
			Entity droppedEntity = *(Entity*)payload->Data;

			if (droppedEntity != entity) {
				droppedEntity.SetParent(entity);
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		ImGui::OpenPopup("Entity settings");

	bool entity_deleted = false;
	if (ImGui::BeginPopup("Entity settings")) {
		if (ImGui::MenuItem("delete"))
			entity_deleted = true;
		ImGui::EndPopup();
	}

	if (opened) {
		for (auto& child : entity.GetChildren()) {
			DrawEntityNode(child);
		}
		ImGui::TreePop();
	}

	if (entity_deleted) {
		m_Context->DestroyEntity(entity);
		if (m_SelectedEntity == entity)
			m_SelectedEntity = {};
	}

	ImGui::PopID();
}

void FE2D::SceneHierarchyPanel::DrawComponents(Entity entity) {
	if (entity.HasComponent<TagComponent>()) {
		auto& tag = entity.GetComponent<TagComponent>().tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			tag = std::string(buffer);
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent")) {
		for (auto& [id, entry] : ComponentFactory::Instance().getRegistredComponents()) {
			entry.drawAddComponentUIFunc(m_SelectedEntity);
		}
		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	entt::registry& registry = m_Context->getRegistry();

	for (auto& [id, entry] : ComponentFactory::Instance().getRegistredComponents()) {
		if (!entry.drawUIFunc)
			continue;

		auto* storage = registry.storage(id);
		if (!storage || !storage->contains(entity))
			continue;

		ImGui::PushID(id);

		constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImGui::Separator();

		bool open = entry.startTreeFunc(registry, entity);

		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

		if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight })) {
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("remove"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open) {
			entry.drawUIFunc(registry, entity, *m_ImGui);
			ImGui::TreePop();
		}

		if (removeComponent)
			entry.removeFunc(registry, entity);

		ImGui::PopID();
	}
}