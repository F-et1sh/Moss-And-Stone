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

	if (this->getSelected() == entity && ImGui::IsKeyDown(ImGuiKey_Delete) && !m_ImGui->IsAnyTextInput())
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

		static FE2D::UUID selected_entity = entity.GetUUID();
		static IStateNode* selected_node = nullptr;
		static AnimationTransition* selected_transition = nullptr;

		if (selected_entity != entity.GetUUID()) { // reset if this is other entity
			selected_node = nullptr;
			selected_transition = nullptr;
		}

		ImVec2 window_size = ImVec2(ImGui::GetContentRegionAvail().x / 3, ImGui::GetContentRegionAvail().y);

		// draw parameters
		ImGui::BeginChild("Parameters", window_size, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);
		{
			static std::string adding_parameter = "DefaultParameter";

			if (ImGui::Button("Add Parameter")) {
				ImGui::OpenPopup("##ADD_PARAMETER");
			}

			if (ImGui::BeginPopup("##ADD_PARAMETER")) {
				if (ImGui::MenuItem("Boolean")) {
					component.parameters.emplace(adding_parameter, AnimationParameter{ static_cast<bool>(false) });
				}
				if (ImGui::MenuItem("Integer")) {
					component.parameters.emplace(adding_parameter, AnimationParameter{ static_cast<int>(0) });
				}
				if (ImGui::MenuItem("Float")) {
					component.parameters.emplace(adding_parameter, AnimationParameter{ static_cast<float> (0.0f) });
				}
				ImGui::EndPopup();
			}

			for (auto& [name, parameter] : component.parameters) {
				if (std::holds_alternative<bool>(parameter.value)) {
					bool value = std::get<bool>(parameter.value) ? 1.0f : 0.0f;
					m_ImGui->CheckBox("Value##AnimationCondition", value);
					parameter.value = value;
				}
				else if (std::holds_alternative<float>(parameter.value)) {
					float value = std::get<float>(parameter.value);
					ImGui::DragFloat("Value##AnimationCondition", &value);
					parameter.value = value;
				}
				else if (std::holds_alternative<int>(parameter.value)) {
					int value = std::get<int>(parameter.value);
					ImGui::DragInt("Value##AnimationCondition", &value);
					parameter.value = value;
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// draw state nodes
		ImGui::BeginChild("State Nodes", window_size, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
				ImGui::OpenPopup("Add Animation");

			if (ImGui::BeginPopup("Add Animation")) {
				std::unique_ptr<IStateNode> node = nullptr;

				if (ImGui::MenuItem("Add Animation State Node")) {
					node = std::make_unique<AnimationStateNode>();
					node->name = "New AnimationState";
				}
				if (ImGui::MenuItem("Add Blend Tree Node")) {
					node = std::make_unique<BlendTreeNode>();
					node->name = "New BlendTree";
				}

				if (node) {
					component.states.emplace_back(std::move(node));

					size_t transitions_size = static_cast<size_t>(component.states.size() / 2);
					if (component.transitions.size() < transitions_size) {
						size_t last = component.states.size() - 1;

						AnimationTransition transition;
						transition.from_state = component.states[last]->name;
						transition.to_state = component.states[last - 1]->name; // can't throw an error

						component.transitions.emplace_back(std::move(transition));
					}
				}

				ImGui::EndPopup();
			}

			for (const auto& state : component.states) {
				if (!state) continue;
				if (ImGui::Button(state->name.c_str())) {
					selected_node = state.get();
					component.current_state = selected_node->name;
					selected_transition = nullptr;
				}

				auto it = std::find_if(component.transitions.begin(), component.transitions.end(), [&](const AnimationTransition& e) {
					return e.from_state == state->name;
					});
				if (it != component.transitions.end()) {
					std::string name = it->from_state + " -> " + it->to_state;
					if (ImGui::Button(name.c_str())) {
						selected_transition = &*it;
						selected_node = nullptr;
					}
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// draw properties
		ImGui::BeginChild("Properties", window_size, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);
		{
			// draw state node properties
			if (selected_node) {
				bool is_deleting = false;

				ImGui::BeginChild("Animation Properties", window_size, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

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

					if (selected_entity != entity.GetUUID()) { // reset if this is other entity
						editing_coords = vec2();
						editing_anim_id = ResourceID<Animation>();
					}

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
						auto transition_it = std::find_if(component.transitions.begin(), component.transitions.end(), [](const AnimationTransition& e) {
							return e.from_state == selected_node->name || e.to_state == selected_node->name;
							});
						if (transition_it != component.transitions.end()) {
							component.transitions.erase(transition_it);
						}

						component.states.erase(it);
						selected_node = nullptr;
					}
				}

				ImGui::EndChild();
			}

			// draw transition properties
			else if (selected_transition) {
				ImGui::BeginChild("Transition Properties", window_size, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

				ImGui::SameLine();

				if (ImGui::Button("Add Condition")) {
					ImGui::OpenPopup("##ADD_CONDITION");
				}
				if (ImGui::BeginPopup("##ADD_CONDITION")) {
					for (const auto& [param_name, param] : component.parameters) {
						if (ImGui::MenuItem((param_name + "##PARAMETER").c_str())) {
							AnimationCondition cond;
							cond.parameter_name = param_name;
							selected_transition->conditions.emplace_back(std::move(cond));
						}
					}
					ImGui::EndPopup();
				}

				for (auto& cond : selected_transition->conditions) {
					if (ImGui::Button(cond.parameter_name.c_str())) {
						ImGui::OpenPopup("##CHOOSE_CONDITION");
					}

					if (ImGui::BeginPopup("##CHOOSE_CONDITION")) {
						for (const auto& [param_name, param] : component.parameters) {
							if (ImGui::MenuItem((param_name + "##PARAMETER").c_str())) {
								cond.parameter_name = param_name;
							}
						}
						ImGui::EndPopup();
					}

					ImGui::SameLine();

					switch (cond.condition_type) {
					case ConditionType::Equals: ImGui::Text("=="); break;
					case ConditionType::Greater: ImGui::Text(">="); break;
					case ConditionType::Less: ImGui::Text("<="); break;
					}

					ImGui::Text(cond.parameter_name.c_str());
				}

				ImGui::EndChild();
			}
		}
		ImGui::EndChild();

		selected_entity = entity.GetUUID(); // set new entity only at the ending

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