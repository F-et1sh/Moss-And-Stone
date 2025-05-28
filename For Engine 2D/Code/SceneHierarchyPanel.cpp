#include "forpch.h"
#include "SceneHierarchyPanel.h"

#include "ContentBrowser.h"

// This needed to disable a security warning on std::strncpy()
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

void FE2D::SceneHierarchyPanel::setContext(Scene& context, IMGUI& imgui, MousePicker& mouse_picker, ContentBrowser& content_browser) {
	m_Context = &context;
	m_ImGui = &imgui;
	m_MousePicker = &mouse_picker;
	m_ContentBrowser = &content_browser;

	this->resetSelected();
}

void FE2D::SceneHierarchyPanel::OnImGuiRender(bool is_preview_hovered, const vec2& preview_mouse_position) {
	ImGui::Begin("Scene Hierarchy", nullptr, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

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
			this->resetSelected();

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

		if (entity_index != -1) {
			this->setSelected({ (entt::entity)entity_index, m_Context });
		}
		else {
			this->resetSelected();
		}
	}

	ImGui::Begin("Properties", nullptr, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);
	if (this->getSelected())
		DrawComponents(this->getSelected());
	ImGui::End();
}

inline Entity FE2D::SceneHierarchyPanel::getSelected() const noexcept {
	return m_Selected;
}

inline void FE2D::SceneHierarchyPanel::setSelected(Entity entity) noexcept {
	m_ContentBrowser->resetSelected();
	m_Selected = entity;
}

inline void FE2D::SceneHierarchyPanel::resetSelected() noexcept {
	m_Selected = {};
}

void FE2D::SceneHierarchyPanel::DrawEntityNode(Entity entity) {
	auto& tag = entity.GetComponent<TagComponent>().tag;
	UUID uuid = entity.GetUUID();

	ImGui::PushID(uuid.get());

	ImGuiTreeNodeFlags flags = ((this->getSelected() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());

	if (ImGui::IsItemClicked()) {
		this->setSelected(entity);
	}
	
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

	if (this->getSelected() == entity && ImGui::IsKeyDown(ImGuiKey_Delete))
		entity_deleted = true;

	if (opened) {
		for (auto& child : entity.GetChildren()) {
			DrawEntityNode(child);
		}
		ImGui::TreePop();
	}

	if (entity_deleted) {
		m_Context->DestroyEntity(entity);
		if (this->getSelected() == entity)
			this->resetSelected();
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
		DisplayAddComponentEntry<PlayerComponent>("PlayerController");
		DisplayAddComponentEntry<VelocityComponent>("Velocity");
		DisplayAddComponentEntry<ColliderComponent>("Collider");
		DisplayAddComponentEntry<AnimatorComponent>("Animator");

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
		m_ImGui->SelectTexture(component.texture.uuid);

		m_ImGui->CheckBox("Flip X", component.flip_x);
		m_ImGui->CheckBox("Flip Y", component.flip_y);
		});

	DrawComponent<CameraComponent>("Camera", entity, [&](auto& component) {
		ImGui::ColorEdit4("Clear Color", (float*)&component.clear_color);
		});

	DrawComponent<PlayerComponent>("PlayerController", entity, [&](auto& component) {
		// ..
		});

	DrawComponent<VelocityComponent>("Velocity", entity, [&](auto& component) {
		m_ImGui->DragVector2("Velocity", component.velocity);
		});

	DrawComponent<ColliderComponent>("Collider", entity, [&](auto& component) {
		m_ImGui->DragVector2("Position", component.position);
		ImGui::Spacing();
		m_ImGui->DragVector2("Size", component.size);

		//m_ImGui->ColliderControl(entity.GetComponent<TransformComponent>(), component);
		});

	DrawComponent<AnimatorComponent>("Animator", entity, [&](auto& component) {
		//ImGui::DragScalar("Current Animation", ImGuiDataType_U64, (unsigned long long*)&component.current_animation, 1.0f, 0, component.animations.size());
		ImGui::DragFloat("Current Time", &component.time);

		const size_t max = component.animations.size();
		const float speed = 1.0f / (max * 10);

		for (size_t i = 0; i < max; i++) {
			ImGui::DragScalar(std::string("Animation " + std::to_string(i + 1)).c_str(), ImGuiDataType_U64, &component.animations[i], speed, 0, &max);
		}

		if (ImGui::Button("Add Animation")) {
			size_t new_anim = component.animations.back() + 1;
			component.animations.push_back(new_anim);
		}
		});
}