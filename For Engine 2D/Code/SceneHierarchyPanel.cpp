#include "forpch.h"
#include "SceneHierarchyPanel.h"

#include "AnimationStateNodes.h"

void FE2D::SceneHierarchyPanel::setContext(Scene& context, IMGUI& imgui, MousePicker& mouse_picker) {
	m_Context = &context;
	m_ImGui = &imgui;
	m_MousePicker = &mouse_picker;

	this->resetSelected();
}

void FE2D::SceneHierarchyPanel::OnImGuiRender(bool is_preview_window_hovered, const vec2& preview_mouse_position) {
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
		DrawComponents(this->getSelected());
	ImGui::End();
}

void FE2D::SceneHierarchyPanel::DrawEntityNode(Entity entity) {
	auto& tag = entity.GetComponent<TagComponent>().tag;
	UUID uuid = entity.GetUUID();

	ImGui::PushID(uuid.get());

	ImGuiTreeNodeFlags flags = ((this->getSelected() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());

	// draw collider
	if (entity.HasComponent<PhysicsComponent>())
		m_ImGui->DrawCollider(entity);

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

	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		this->setSelected(entity);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		ImGui::OpenPopup("Entity settings");

	bool entity_deleted = false;
	if (ImGui::BeginPopup("Entity settings")) {
		if (ImGui::MenuItem("Delete"))
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
		DisplayAddComponentEntry<SpriteComponent>("Sprite Renderer");
		DisplayAddComponentEntry<CameraComponent>("Camera");
		DisplayAddComponentEntry<PhysicsComponent>("Physics");
		DisplayAddComponentEntry<AnimatorComponent>("Animator");
		DisplayAddComponentEntry<NativeScriptComponent>("Script");

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

		m_ImGui->TransformControl(entity);
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

	DrawComponent<PhysicsComponent>("Physics", entity, [&](auto& component) {
		m_ImGui->DragVector2("Position", component.position);
		ImGui::Spacing();
		m_ImGui->DragVector2("Size", component.size);
		ImGui::Spacing();
		ImGui::DragFloat("Bounce Factor", &component.bounce_factor, 0.1f);
		ImGui::Spacing();
		ImGui::DragFloat("Mass", &component.mass, 1);

		m_ImGui->CheckBox("Is Trigger", component.is_trigger);
		m_ImGui->CheckBox("Is Static", component.is_static);

		m_ImGui->DragVector2("Velocity", component.velocity);

		ImGui::Spacing();

		ImGui::Text("Colliding entities");

		for (auto e : component.entities_in) {
			ImGui::PushID(e.GetComponent<IDComponent>().id.get());
			ImGui::Text(e.GetComponent<TagComponent>().tag.c_str());
			ImGui::PopID();
		}
		});

	DrawComponent<AnimatorComponent>("Animator", entity, [&](auto& component) {
		ImGui::Begin("Animator Editor");
		
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
			ImGui::OpenPopup("Add Animation");

		if (ImGui::BeginPopup("Add Animation")) {
			if (ImGui::MenuItem("Add Animation Node")) {
				auto& node = component.states.emplace_back(std::make_unique<AnimationStateNode>());
				node->name = "New AnimationState";
			}
			if (ImGui::MenuItem("Add Blend Tree Node")) {
				auto& node = component.states.emplace_back(std::make_unique<BlendTreeNode>());
				node->name = "New BlendTree";
			}
			ImGui::EndPopup();
		}

		static IStateNode* selected_node = nullptr;
		for (const auto& state : component.states) {
			if (!state) continue;
			if (ImGui::Button(state->name.c_str())) {
				selected_node = state.get();
				component.current_state = selected_node->name;
			}
		}

		if (selected_node) {
			bool is_deleting = false;
			
			ImGui::SameLine();

			ImVec2 window_pos = ImGui::GetCursorPos();
			window_pos.x += ImGui::GetContentRegionAvail().x / 2;
			ImGui::SetCursorPos(window_pos);
			ImGui::BeginChild("Animation Properties", ImVec2(0, 0), m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

			if (auto node = dynamic_cast<AnimationStateNode*>(selected_node)) {
				m_ImGui->SelectAnimation(node->animation_id);
				m_ImGui->DrawAnimation(node->animation_id);
				
				ImGui::Separator();

				m_ImGui->InputText("Node Name", node->name);

				ImGui::SameLine();
				if (ImGui::Button("Remove")) is_deleting = true;
				
				ImGui::Checkbox("Looping", &node->looping);
				if (node->looping)
					ImGui::DragFloat("Duration", &node->duration, 0.1f);
			}
			else if (auto node = dynamic_cast<BlendTreeNode*>(selected_node)) {
				static vec2 editing_coords = vec2();
				static ResourceID<Animation> editing_anim_id;

				m_ImGui->InputText("Node Name", node->name);
				
				ImGui::SameLine();
				if (ImGui::Button("Remove")) is_deleting = true;

				ImGui::Checkbox("Looping", &node->looping);

				if (node->looping)
					ImGui::DragFloat("Duration", &node->duration, 0.1f);

				ImGui::Separator();

				m_ImGui->DragVector2("Coordinate", editing_coords);
				
				ImGui::SameLine();
				
				m_ImGui->SelectAnimation(editing_anim_id);
				m_ImGui->DrawAnimation(editing_anim_id);

				if (ImGui::Button("Add Point"))
					node->animation_points.emplace_back(editing_coords, editing_anim_id);

				size_t i = 0;
				int deleting_index = -1;

				for (const auto& [coords, anim_id] : node->animation_points) {
					ImGui::Text("%f x %f", coords.x, coords.y);

					ImGui::SameLine();

					m_ImGui->DrawAnimation(anim_id);

					ImGui::SameLine();

					if (ImGui::Button(("Remove##" + std::to_string(i)).c_str())) deleting_index = i;

					ImGui::SameLine();

					if (ImGui::Button("Edit")) {
						editing_coords = coords;
						editing_anim_id = anim_id;
						deleting_index = i;
					}

					i++;
				}
				if (deleting_index != -1)
					node->animation_points.erase(node->animation_points.begin() + deleting_index);
			}

			if (is_deleting) {
				auto it = std::find_if(component.states.begin(), component.states.end(), [](const std::unique_ptr<IStateNode>& state) { return state.get() == selected_node; });
				if (it != component.states.end()) {
					component.states.erase(it);
					selected_node = nullptr;
				}
			}
			
			ImGui::EndChild();
		}

		ImGui::End();
		});

	DrawComponent<NativeScriptComponent>("Script", entity, [&](auto& component) {
		if (ImGui::Button("Change script"))
			ImGui::OpenPopup("ScriptList");

		if (ImGui::BeginPopup("ScriptList")) {
			const auto& scripts = ScriptFactory::Instance().GetRegisteredScripts();
			for (const auto& pair : scripts) {
				std::string script_name = pair.first;

				if (ImGui::MenuItem(script_name.c_str())) {
					component.instance = ScriptFactory::Instance().CreateScript(script_name);
					component.instance->setContext(entity);
					component.script_name = script_name;
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();

		if (component.instance)
			component.instance->OnEditorPanel(*this->m_ImGui);
		});
}