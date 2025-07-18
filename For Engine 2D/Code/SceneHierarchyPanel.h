#pragma once
#include "MousePicker.h"
#include "ProjectVariables.h"

// Based on Hazel ( The Cherno ) - Apache License 2.0

namespace FE2D {
	class FOR_API ContentBrowser;

	class FOR_API SceneHierarchyPanel {
	public:
		SceneHierarchyPanel(Scene* context) : m_Context(context) { }
		SceneHierarchyPanel() = default;
		~SceneHierarchyPanel() = default;

		void setContext(Scene& context, IMGUI& imgui, MousePicker& mouse_picker);

		void OnImGuiRender(bool is_preview_window_hovered, const vec2& preview_mouse_position, bool is_game_running);

		inline Entity getSelected() const noexcept { return this->m_Selected; }
		inline void setSelected(Entity entity)noexcept { this->m_Selected = entity; }
		inline void resetSelected()noexcept { this->m_Selected = {}; }
	
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	
		template<typename T, typename UIFunction>
		void DrawComponent(const std::string& name, Entity entity, UIFunction ui_function) {
			if (!entity.HasComponent<T>()) return;

			auto& component = entity.GetComponent<T>();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::Separator();
			
			std::string tree_node_name = name;
			if constexpr (std::is_same_v<T, NativeScriptComponent>)
				tree_node_name += " (" + component.script_name + ")";
			
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, tree_node_name.c_str());
			ImGui::PopStyleVar();
			ImVec2 content_region_available = ImGui::GetContentRegionAvail();
			ImGui::SameLine(content_region_available.x - lineHeight * 0.5f);
			if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight })) {
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open) {
				ui_function(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
		
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entry_name) {
			if (m_Selected.HasComponent<T>()) return;

			if (ImGui::MenuItem(entry_name.c_str())) {
				m_Selected.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}

	private:
		Scene* m_Context = nullptr;
		Entity m_Selected;

		IMGUI* m_ImGui = nullptr;
		MousePicker* m_MousePicker = nullptr;
	
		friend class FOR_API ContentBrowser;
	};
}