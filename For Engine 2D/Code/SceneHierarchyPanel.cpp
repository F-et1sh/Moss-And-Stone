#include "forpch.h"
#include "SceneHierarchyPanel.h"

#include "ContentBrowser.h"
#include "Scene.h"

void FE2D::SceneHierarchyPanel::setContext(Scene& context, IMGUI& imgui, MousePicker& mouse_picker) {
	m_Context = &context;
	m_ImGui = &imgui;
	m_MousePicker = &mouse_picker;

	this->resetSelected();
}

void FE2D::SceneHierarchyPanel::OnImGuiRender(bool is_preview_window_focused, bool is_preview_window_hovered, const vec2& preview_mouse_position) {
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

		// right-click on blank space
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("New Entity");

			ImGui::EndPopup();
		}

		for (auto system : m_Context->m_SystemsList) system->OnPropertiesPanel(*m_ImGui);
	}
	ImGui::End();

	if (!m_ImGui->IsAnyGizmoHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_preview_window_hovered) {
		int entity_index = m_MousePicker->ReadPixel(preview_mouse_position);

		if (entity_index != -1) this->setSelected({ (entt::entity)entity_index, m_Context });
		else					this->resetSelected();
	}

	ImGui::Begin("Properties", nullptr, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);
	if (this->getSelected())
		DrawComponents(this->getSelected(), is_preview_window_focused);
	ImGui::End();
}

void FE2D::SceneHierarchyPanel::DrawEntityNode(Entity entity) {
	auto& tag = entity.GetComponent<TagComponent>().tag;
	UUID uuid = entity.GetUUID();

	ImGui::PushID(uuid.get());

	ImGuiTreeNodeFlags flags = ((this->getSelected() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("ENTITY_DRAGGING", &entity, sizeof(Entity));
		ImGui::Text("%s", tag.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAGGING")) {
			Entity dropped_entity = *static_cast<Entity*>(payload->Data);

			if (dropped_entity != entity)
				dropped_entity.SetParent(entity);
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) 
		this->setSelected(entity);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		ImGui::OpenPopup("Entity settings");

	bool entity_deleted = false;
	if (ImGui::BeginPopup("Entity settings")) {
		if (ImGui::MenuItem("Delete"))
			entity_deleted = true;
		ImGui::EndPopup();
	}

	if (ImGui::IsWindowFocused() && this->getSelected() == entity && ImGui::IsKeyDown(ImGuiKey_Delete))
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

void FE2D::SceneHierarchyPanel::DrawComponents(Entity entity, bool is_preview_window_focused) {
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
		DisplayAddComponentEntry<SpriteComponent>("Sprite Renderer");
		DisplayAddComponentEntry<CameraComponent>("Camera");
		DisplayAddComponentEntry<PlayerComponent>("Player Controller");
		DisplayAddComponentEntry<VelocityComponent>("Velocity");
		DisplayAddComponentEntry<ColliderComponent>("Collider");
		DisplayAddComponentEntry<CharacterAnimatorComponent>("Character Animator");

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

		if (is_preview_window_focused) m_ImGui->TransformControl(component);
		});

	DrawComponent<SpriteComponent>("Sprite Renderer", entity, [&](auto& component) {
		m_ImGui->SelectTexture(component.texture);

		auto& texture = m_ImGui->getResourceManager()->GetResource(component.texture);
		component.frame = vec4(0, 0, texture.getSize());

		m_ImGui->CheckBox("Flip X", component.flip_x);
		m_ImGui->CheckBox("Flip Y", component.flip_y);
		});

	DrawComponent<CameraComponent>("Camera", entity, [&](auto& component) {
		ImGui::ColorEdit4("Clear Color", (float*)&component.clear_color);
		});

	DrawComponent<PlayerComponent>("Player Controller", entity, [&](auto& component) {
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

	DrawComponent<CharacterAnimatorComponent>("Character Animator", entity, [&](auto& component) {
		static ResourceID<Animation> editing_animation;
		static std::string editing_name = "";
		static char name_buffer[128] = "";

		if (!editing_name.empty()) {
			ImGui::InputText("Animation Name", name_buffer, sizeof(name_buffer));

			m_ImGui->SelectAnimation(editing_animation);
			auto it = component.animations.find(editing_name);
			if (it != component.animations.end()) {
				component.animations.erase(it);
				editing_name = name_buffer;
				if (editing_name.empty()) editing_name = " ";
				component.animations.emplace(editing_name, editing_animation);
			}

			if (ImGui::IsKeyDown(ImGuiKey_Delete)) {
				auto it = component.animations.find(editing_name);
				if (it != component.animations.end())
					component.animations.erase(it);

				editing_animation = ResourceID<Animation>();
				editing_name = "";
				memset(name_buffer, 0, sizeof(name_buffer));
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Add Animation"))
			component.animations.emplace("New Animation", ResourceID<Animation>(FE2D::UUID(0)));

		constexpr float spacing = 20.0f;
		constexpr ImVec2 card_size = ImVec2(100, 130);
		constexpr ImVec4 card_bg_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		constexpr ImVec4 card_bg_hovered = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		constexpr ImVec4 card_bg_selected = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

		ImVec2 pos = ImGui::GetCursorScreenPos();
		int index = 0;

		bool is_any_card_hovered = false;

		for (auto& [name, anim_id] : component.animations) {
			ImGui::PushID(index);

			float offset_x = index * (card_size.x + spacing);
			ImVec2 card_pos = ImVec2(pos.x + offset_x, pos.y);
			ImGui::SetCursorScreenPos(card_pos);

			ImVec2 cursor = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			ImVec2 card_min = cursor;
			ImVec2 card_max = ImVec2(cursor.x + card_size.x, cursor.y + card_size.y);

			bool hovered = ImGui::IsMouseHoveringRect(card_min, card_max);
			if (hovered) is_any_card_hovered = true;
			bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

			if (clicked) {
				strncpy_s(name_buffer, name.c_str(), sizeof(name_buffer));
				editing_name = name;
				editing_animation = anim_id;

				component.current_animation = anim_id;
			}

			draw_list->AddRectFilled(
				card_min, card_max,
				ImGui::GetColorU32(editing_name == name ? card_bg_selected : (hovered ? card_bg_hovered : card_bg_color)),
				4.0f
			);

			ImGui::SetCursorScreenPos(ImVec2(cursor.x + 5, cursor.y + 5));
			ImGui::Text(name.c_str());

			ImVec2 preview_pos = ImVec2(cursor.x + 5, cursor.y + 25);
			ImGui::SetCursorScreenPos(preview_pos);

			if (anim_id.uuid != FE2D::UUID(0)) m_ImGui->DrawAnimation(anim_id);
			else ImGui::Dummy(ImVec2(card_size.x - 10, card_size.x - 10));

			ImGui::PopID();
			index++;
		}

		if (!is_any_card_hovered && ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) editing_name.clear();
		});
}