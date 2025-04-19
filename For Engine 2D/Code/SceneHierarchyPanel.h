#pragma once
#include "MousePicker.h"

// Based on Hazel ( The Cherno ) - Apache License 2.0

namespace FE2D {
	class FOR_API SceneHierarchyPanel {
	public:
		SceneHierarchyPanel(Scene* context) :
			m_Context(context) { }
		SceneHierarchyPanel() = default;
		~SceneHierarchyPanel() = default;

		void setContext(Scene* context, IMGUI* imgui, MousePicker* mouse_picker);

		void OnImGuiRender(bool is_preview_hovered, const vec2& preview_mouse_position);

		Entity getSelectedEntity() const { return m_SelectedEntity; }
		inline void setSelectedEntity(Entity entity)noexcept { m_SelectedEntity = entity; }

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Scene* m_Context = nullptr;
		Entity m_SelectedEntity;

		IMGUI* m_ImGui = nullptr;
		MousePicker* m_MousePicker = nullptr;
	};
}