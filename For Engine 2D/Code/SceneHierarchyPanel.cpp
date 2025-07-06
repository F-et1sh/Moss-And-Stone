#include "forpch.h"
#include "SceneHierarchyPanel.h"

#include "AnimationStateNodes.h"

void FE2D::SceneHierarchyPanel::setContext(Scene& context, IMGUI& imgui, MousePicker& mouse_picker) {
	m_Context = &context;
	m_ImGui = &imgui;
	m_MousePicker = &mouse_picker;

	this->resetSelected();
}

void FE2D::SceneHierarchyPanel::OnImGuiRender(bool is_preview_window_hovered, const vec2& preview_mouse_position, bool is_game_running) {
	ImGui::Begin("Scene Hierarchy", nullptr, m_ImGui->IsAnyGizmoHovered() ? ImGuiWindowFlags_NoMove : 0);

	if (m_Context) {
		const auto& view = m_Context->getRegistry().view<IDComponent>();

		// this needed to draw entities after loop, where was used entt::registry::view()
		// because you can't modify entities in this loop
		std::vector<Entity> entities_to_draw;
		entities_to_draw.reserve(view.size());

		for (auto entity_handle : view) {
			Entity entity = { entity_handle, m_Context };

			if (entity.HasParent()) continue;

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

	if (!m_ImGui->IsAnyGizmoHovered() && 
		ImGui::IsMouseClicked(ImGuiMouseButton_Left) && 
		is_preview_window_hovered && 
		!is_game_running) {

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
		
		auto& physics_layers = m_Context->m_ProjectVariables->getPhysicsLayers();

		std::string_view layer_button_name = "Layer";
		
		auto name = physics_layers.get_name_by_mask(component.layer);
		if (!name.empty()) layer_button_name = name;
		
		if (ImGui::Button(layer_button_name.data())) {
			ImGui::OpenPopup("##LAYER_CHOOSING");
		}

		m_ImGui->CheckBox("Is Trigger", component.is_trigger);
		m_ImGui->CheckBox("Is Static", component.is_static);

		m_ImGui->DragVector2("Velocity", component.velocity);

		if (ImGui::BeginPopup("##LAYER_CHOOSING")) {
			size_t i = 0;
			for (auto& info : physics_layers.getLayers()) {
				if (info.name.empty()) continue;
				if (ImGui::MenuItem((info.name + "##LAYER_" + std::to_string(i)).c_str())) {
					component.layer = i;
				}
				i++;
			}
			
			ImGui::EndPopup();
		}

		ImGui::Spacing();

		ImGui::Text("Colliding entities");

		for (auto e : component.entities_in) {
			ImGui::PushID(e.GetComponent<IDComponent>().id.get());
			ImGui::Text(e.GetComponent<TagComponent>().tag.c_str());
			ImGui::PopID();
		}
		});

	DrawComponent<AnimatorComponent>("Animator", entity, [&](auto& component) {
		static FE2D::UUID selected_entity = entity.GetUUID();
		static IStateNode* selected_node = nullptr;
		static size_t selected_node_index = 0;
		static AnimationTransition* selected_transition = nullptr;

		if (selected_entity != entity.GetUUID()) { // reset if this is other entity
			selected_node = nullptr;
			selected_node_index = 0;
			selected_transition = nullptr;
		}

		ImVec2 window_size = ImVec2(ImGui::GetContentRegionAvail().x / 3, 400);
		ImVec2 window_pos = ImGui::GetCursorPos();

		// draw parameters
		ImGui::BeginChild("Parameters", window_size, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX);
		{
			static std::string adding_parameter_name = "DefaultParameter";
			static AnimationParameter adding_parameter;

			if (selected_entity != entity.GetUUID()) {
				adding_parameter_name = "DefaultParameter";
				adding_parameter = {};
			}

			auto draw_parameter_input = [](const std::string& label, AnimationParameter& parameter) {
				if (std::holds_alternative<bool>(parameter.value))
				{
					float item_width = ImGui::GetFrameHeight();
					ImGui::PushItemWidth(item_width);

					bool value = std::get<bool>(parameter.value);
					ImGui::Checkbox((label + "##Boolean").c_str(), &value);
					parameter.value = value;

					ImGui::PopItemWidth();
				}
				else if (std::holds_alternative<float>(parameter.value))
				{
					float item_width = ImGui::GetFrameHeight() * 4;
					ImGui::PushItemWidth(item_width);

					float value = std::get<float>(parameter.value);
					ImGui::DragFloat((label + "##Float").c_str(), &value);
					parameter.value = value;

					ImGui::PopItemWidth();
				}
				else if (std::holds_alternative<int>(parameter.value))
				{
					float item_width = ImGui::GetFrameHeight() * 4;
					ImGui::PushItemWidth(item_width);

					int value = std::get<int>(parameter.value);
					ImGui::DragInt((label + "##Integer").c_str(), &value);
					parameter.value = value;

					ImGui::PopItemWidth();
				}
				else if (std::holds_alternative<Trigger>(parameter.value))
				{
					float item_width = ImGui::GetFrameHeight();
					ImGui::PushItemWidth(item_width);

					Trigger value = std::get<Trigger>(parameter.value);
					if (ImGui::Button((label + "##Trigger").c_str())) value.trigger();
					parameter.value = value;

					ImGui::PopItemWidth();
				}
				};

			// draw InputText window and text itself
			float text_width = ImGui::GetContentRegionAvail().x - 130;
			m_ImGui->InputText("Parameter Name", adding_parameter_name, text_width);
			if (ImGui::Button("Change Type")) {
				ImGui::OpenPopup("##CHOOSE_PARAMETER_TYPE");
			}

			draw_parameter_input(adding_parameter_name, adding_parameter);

			static constexpr ImVec2 button_size = ImVec2(110, 25);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - button_size.x);

			// draw Add Parameter button
			if (ImGui::Button("Add Parameter", button_size)) {
				component.parameters.emplace(adding_parameter_name, adding_parameter);
			}

			if (ImGui::BeginPopup("##CHOOSE_PARAMETER_TYPE")) {
				if (ImGui::MenuItem("Boolean")) {
					adding_parameter = AnimationParameter{ static_cast<bool>(false) };
				}
				if (ImGui::MenuItem("Float")) {
					adding_parameter = AnimationParameter{ static_cast<float> (0.0f) };
				}
				if (ImGui::MenuItem("Integer")) {
					adding_parameter = AnimationParameter{ static_cast<int>(0) };
				}
				if (ImGui::MenuItem("Trigger")) {
					adding_parameter = AnimationParameter{ static_cast<Trigger>(0.0f) };
				}
				ImGui::EndPopup();
			}

			std::string parameter_to_delete;
			for (auto& [name, parameter] : component.parameters) {
				std::string label = std::string(name + "##Parameter");

				draw_parameter_input(label, parameter);

				ImGui::SameLine();

				if (ImGui::Button(("Remove##" + name).c_str())) {
					parameter_to_delete = name;
				}
			}
			
			if (!parameter_to_delete.empty())
				component.parameters.erase(parameter_to_delete);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(window_pos.x + window_size.x, window_pos.y));

		// draw state nodes
		ImGui::BeginChild("State Nodes", ImVec2(window_size.x, window_size.y / 2), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX);
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
				ImGui::OpenPopup("##ADD_ANIMATION");

			if (ImGui::BeginPopup("##ADD_ANIMATION")) {
				std::unique_ptr<IStateNode> node = nullptr;

				if (ImGui::MenuItem("Add Animation State Node")) node = std::make_unique<AnimationStateNode>();
				if (ImGui::MenuItem("Add Blend Tree Node"))		 node = std::make_unique<BlendTreeNode>();

				if (node) {
					component.states.emplace_back(std::move(node));

					size_t needed_transitions_size = static_cast<size_t>(component.states.size() / 2);
					if (component.transitions.size() < needed_transitions_size) {
						size_t last = component.states.size() - 1;

						AnimationTransition transition;
						transition.from_state = last - 1;
						transition.to_state = last;

						component.transitions.emplace_back(std::move(transition));
					}
				}

				ImGui::EndPopup();
			}

			size_t i = 0;
			for (const auto& state : component.states) {
				if (!state) continue;
				if (ImGui::Button(state->name.c_str())) {
					selected_node = state.get();
					selected_node_index = i;
					selected_transition = nullptr;
					component.current_state = i;
				}

				if (component.current_state == i) {
					ImGui::SameLine();
					ImGui::Text(" <-");
				}

				i++;
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(window_pos.x + window_size.x, window_pos.y + (window_size.y / 2)));

		// draw transitions
		ImGui::BeginChild("Transitions", ImVec2(window_size.x, window_size.y / 2), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX);
		{
			static size_t from_state_index = 0;
			static size_t to_state_index = 1;

			if (selected_entity != entity.GetUUID()) {
				from_state_index = 0;
				to_state_index = 1;
			}

			std::string from_state_name = "None##from";
			std::string to_state_name = "None##to";

			if (from_state_index < component.states.size() &&
				to_state_index < component.states.size()) {

				from_state_name = component.states[from_state_index]->name;
				to_state_name = component.states[to_state_index]->name;
			}

			if (ImGui::Button(from_state_name.c_str())) ImGui::OpenPopup("##CHOOSE_FROM_STATE");
			
			ImGui::SameLine();
			
			ImGui::Text(" -> ");

			ImGui::SameLine();
			
			if (ImGui::Button(to_state_name.c_str())) ImGui::OpenPopup("##CHOOSE_TO_STATE");

			if (ImGui::BeginPopup("##CHOOSE_FROM_STATE"))
			{	
				size_t start_index = from_state_index;
				
				size_t i = 0;
				for (const auto& state : component.states) {
					if (ImGui::MenuItem(state->name.c_str())) from_state_index = i;
					i++;
				}

				if (to_state_index == from_state_index) to_state_index = start_index;

				ImGui::EndPopup();
			}
			if (ImGui::BeginPopup("##CHOOSE_TO_STATE"))
			{	
				size_t start_index = to_state_index;

				size_t i = 0;
				for (const auto& state : component.states) {
					if (ImGui::MenuItem(state->name.c_str())) to_state_index = i;
					i++;
				}

				if (from_state_index == to_state_index) from_state_index = start_index;

				ImGui::EndPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Add Transition"))
				selected_transition = &component.transitions.emplace_back(AnimationTransition{ from_state_index, to_state_index });

			ImGui::Separator();

			size_t i = 0;
			int transition_to_delete = -1;
			for (auto& transition : component.transitions) {
				// tr - transition
				std::string tr_from_state_name = component.states[transition.from_state]->name;
				std::string tr_to_state_name = component.states[transition.to_state]->name;
				std::string tr_button_name = (tr_from_state_name + " -> " + tr_to_state_name);

				if (ImGui::Button(tr_button_name.c_str())) {
					selected_transition = &transition;
					selected_node = nullptr;
				}

				ImGui::SameLine();

				if (ImGui::Button(("Remove##" + tr_button_name).c_str())) {
					transition_to_delete = i;

					if (selected_transition == &transition)
						selected_transition = nullptr;

					break;
				}

				i++;
			}

			if (transition_to_delete != -1)
				component.transitions.erase(component.transitions.begin() + transition_to_delete);
		}
		ImGui::EndChild();
		
		ImGui::SetCursorPos(ImVec2(window_pos.x + window_size.x * 2, window_pos.y));

		// draw properties
		ImGui::BeginChild("Properties", window_size, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX);
		{
			// draw state node properties
			if (selected_node) {
				bool is_deleting = false;

				if (auto node = dynamic_cast<AnimationStateNode*>(selected_node)) {
					m_ImGui->SelectAnimation(node->animation_id);
					m_ImGui->DrawAnimation(node->animation_id);

					ImGui::Separator();

					m_ImGui->InputText("Node Name", node->name);

					ImGui::SameLine();
					if (ImGui::Button("Remove")) is_deleting = true;

					ImGui::Checkbox("Looping", &node->looping);
				}
				else if (auto node = dynamic_cast<BlendTreeNode*>(selected_node)) {
					static vec2 editing_coords = vec2();
					static ResourceID<Animation> editing_anim_id;

					if (selected_entity != entity.GetUUID()) { // reset if this is other entity
						editing_coords = vec2();
						editing_anim_id = ResourceID<Animation>();
					}

					m_ImGui->InputText("Node Name", node->name, node->name.size() * 10);

					auto choose_coord_parameter = [component](std::string& parameter_name, const std::string& axis) {
						ImGui::Text((axis + " : ").c_str());

						ImGui::SameLine();

						if (ImGui::Button(parameter_name.empty() ? ("None##" + axis).c_str() : (parameter_name + "##PARAMETER_CHOOSING_" + axis).c_str()))
							ImGui::OpenPopup(("##CHOOSE_COORD_" + axis).c_str());

						if (ImGui::BeginPopup(("##CHOOSE_COORD_" + axis).c_str())) {
							for (const auto& [name, parameter] : component.parameters) {
								if (ImGui::MenuItem((name + "##BLEND_PARAMETER_" + axis).c_str()))
									parameter_name = name;
							}
							ImGui::EndPopup();
						}
						};

					choose_coord_parameter(node->parameter_name_x, "X");
					choose_coord_parameter(node->parameter_name_y, "Y");

					ImGui::Separator();

					if (ImGui::Button("Remove")) is_deleting = true;

					ImGui::Checkbox("Looping", &node->looping);

					ImGui::Separator();

					m_ImGui->DragVector2("Coordinate", editing_coords);

					ImGui::SameLine();

					m_ImGui->SelectAnimation(editing_anim_id);
					m_ImGui->DrawAnimation(editing_anim_id);

					if (ImGui::Button("Add Point"))
						node->animation_points.emplace_back(editing_coords, editing_anim_id);

					size_t i = 0;
					int deleting_index = -1;

					for (auto& [coords, anim_id] : node->animation_points) {
						ImGui::Text("%f x %f", coords.x, coords.y);

						ImGui::SameLine();

						m_ImGui->DrawAnimation(anim_id);

						ImGui::SameLine();

						if (ImGui::Button(("Remove##" + std::to_string(i)).c_str())) deleting_index = i;
						if (ImGui::Button(("Normalize##" + std::to_string(i)).c_str())) {
							if (coords.length() != 0.0f)
								coords = normalize(coords);
						}

						ImGui::SameLine();

						if (ImGui::Button(("Edit##" + std::to_string(i)).c_str())) {
							editing_coords = coords;
							editing_anim_id = anim_id;
							deleting_index = i;
						}

						i++;
					}
					if (deleting_index != -1)
						node->animation_points.erase(node->animation_points.begin() + deleting_index);
				}

				if (is_deleting) { // call this only at the ending
					for (auto it = component.transitions.begin(); it != component.transitions.end(); ) {
						if (it->from_state == selected_node_index || it->to_state == selected_node_index) {
							it = component.transitions.erase(it);
							continue;
						}

						if (it->from_state > selected_node_index) it->from_state--;
						if (it->to_state   > selected_node_index) it->to_state--;

						it++;
					}

					component.states.erase(component.states.begin() + selected_node_index);

					selected_node = nullptr;
					selected_transition = nullptr;
				}
			}

			// draw transition properties
			else if (selected_transition) {
				if (ImGui::Button("Add Condition")) {
					ImGui::OpenPopup("##ADD_CONDITION");
				}
				if (ImGui::BeginPopup("##ADD_CONDITION")) {
					if (component.parameters.empty())
						ImGui::Text("Please add at least one parameter on the left panel");

					for (const auto& [param_name, param] : component.parameters) {
						auto it = std::find_if(selected_transition->conditions.begin(), selected_transition->conditions.end(), [param_name](const AnimationCondition& e) { 
							return e.parameter_name == param_name;
							});
						if (it != selected_transition->conditions.end())
							continue;

						if (ImGui::MenuItem((param_name + "##PARAMETER").c_str())) {
							AnimationCondition cond;
							cond.parameter_name = param_name;
							selected_transition->conditions.emplace_back(std::move(cond));
						}
					}
					ImGui::EndPopup();
				}

				size_t i = 0;
				int index_to_delete = -1;
				for (auto& cond : selected_transition->conditions) {
					auto it = std::find_if(component.parameters.begin(), component.parameters.end(), [cond](const std::pair<std::string, AnimationParameter>& e) {
						return e.first == cond.parameter_name;
						});
					if (it == component.parameters.end()) {
						index_to_delete = i;
						break;
					}
					AnimationParameter anim_param = component.parameters[cond.parameter_name];

					ImGui::Text(cond.parameter_name.c_str());

					ImGui::SameLine();

					// draw condition mark ( ==, >=, <= )
					if (!std::holds_alternative<Trigger>(anim_param.value)) { // don't draw the mark if it's a trigger
						std::string cond_text;
						switch (cond.condition_type) {
							case ConditionType::Equals : cond_text = " == "; break;
							case ConditionType::Greater: cond_text = " >= "; break;
							case ConditionType::Less   : cond_text = " <= "; break;
						}

						if (ImGui::Button(cond_text.c_str())) {
							ImGui::OpenPopup("##CHOOSE_CONDITION_TYPE");
						}

						if (ImGui::BeginPopup("##CHOOSE_CONDITION_TYPE")) {
							if (ImGui::MenuItem("Equals" )) cond.condition_type = ConditionType::Equals;
							if (ImGui::MenuItem("Greater")) cond.condition_type = ConditionType::Greater;
							if (ImGui::MenuItem("Less"	 )) cond.condition_type = ConditionType::Less;
							ImGui::EndPopup();
						}
					}

					ImGui::SameLine();

					ImGui::PushID(("##ConditionValue_" + cond.parameter_name).c_str());

					if (std::holds_alternative<bool>(anim_param.value))
					{
						float item_width = ImGui::GetFrameHeight();
						ImGui::PushItemWidth(item_width);

						bool value = static_cast<bool>(cond.value);
						ImGui::Checkbox(("##Cond_" + cond.parameter_name).c_str(), &value);
						cond.value = value;

						ImGui::PopItemWidth();
					}
					else if (std::holds_alternative<float>(anim_param.value))
					{
						float item_width = ImGui::GetFrameHeight() * 4;
						ImGui::PushItemWidth(item_width);

						float value = static_cast<float>(cond.value);
						ImGui::DragFloat(("##Cond_" + cond.parameter_name).c_str(), &value);
						cond.value = value;

						ImGui::PopItemWidth();
					}
					else if (std::holds_alternative<int>(anim_param.value))
					{
						float item_width = ImGui::GetFrameHeight() * 4;
						ImGui::PushItemWidth(item_width);

						int value = static_cast<int>(cond.value);
						ImGui::DragInt(("##Cond_" + cond.parameter_name).c_str(), &value);
						cond.value = value;

						ImGui::PopItemWidth();
					}
					else if (std::holds_alternative<Trigger>(anim_param.value))
					{
						cond.condition_type = ConditionType::Equals;
						
						if (ImGui::Button(!cond.value ? "When Not Triggered" : "When Triggered"))
							cond.value = !cond.value;
					}

					ImGui::PopID();

					i++;
				}

				if (index_to_delete != -1) {
					selected_transition->conditions.erase(selected_transition->conditions.begin() + index_to_delete);
				}
			}
		}
		ImGui::EndChild();


		selected_entity = entity.GetUUID(); // set new entity only at the ending
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