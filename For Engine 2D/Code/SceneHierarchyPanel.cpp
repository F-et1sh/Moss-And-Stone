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

void FE2D::SceneHierarchyPanel::OnImGuiRender(bool is_preview_hovered, const vec2& preview_mouse_position) {
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

	if (!m_ImGui->IsAnyGizmoHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_preview_hovered) {
		int entity_index = m_MousePicker->ReadPixel(preview_mouse_position);

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

	if (m_SelectedEntity == entity && ImGui::IsKeyDown(ImGuiKey_Delete))
		entity_deleted = true;

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
		DisplayAddComponentEntry<TransformComponent>("Transform");
		DisplayAddComponentEntry<SpriteComponent>("SpriteRenderer");
		DisplayAddComponentEntry<CameraComponent>("Camera");

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	DrawComponent<TransformComponent>("Transform", entity, [&](auto& component) {
		m_ImGui->DragVector2("Position", component.position);
		ImGui::Spacing();
		m_ImGui->DragInt("Layer", component.layer);
		ImGui::Spacing();
		m_ImGui->CheckBox("Auto-Sotring", component.auto_sort);
		ImGui::Spacing();
		m_ImGui->DragVector2("Scale", component.scale);
		ImGui::Spacing();
		m_ImGui->DragFloat("Rotation", component.rotation);
		ImGui::Spacing();
		m_ImGui->DragVector2("Origin", component.origin);

		m_ImGui->TransformControl(component);
		});

	DrawComponent<SpriteComponent>("SpriteRenderer", entity, [&](auto& component) {
		// TODO : rewrite texture selection

		auto texture_optional = m_ImGui->SelectTexture();
		if (texture_optional.has_value()) {
			Texture& texture = texture_optional.value().second;

			component.texture_coords.x = 0;
			component.texture_coords.y = 0;
			component.texture_coords.z = texture.getSize().x;
			component.texture_coords.w = texture.getSize().y;

			component.texture_index = texture_optional.value().first;
		}

		m_ImGui->CheckBox("Flip X", component.flip_x);
		m_ImGui->CheckBox("Flip Y", component.flip_y);
		});

	DrawComponent<CameraComponent>("Camera", entity, [&](auto& component) {
		ImGui::ColorEdit4("Clear Color", (float*)&component.clear_color);
		});
}