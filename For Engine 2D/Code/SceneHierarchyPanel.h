#pragma once

// Based on Hazel ( The Cherno ) - Apache License 2.0

namespace FE2D {
	class FOR_API SceneHierarchyPanel {
	public:
		SceneHierarchyPanel(Scene* context) :
			m_Context(context) { }
		SceneHierarchyPanel() = default;
		~SceneHierarchyPanel() = default;

		void setContext(Scene* scene);

		void OnImGuiRender();

		Entity getSelectedEntity() const { return m_SelectedEntity; }
		inline void setSelectedEntity(Entity entity)noexcept {
			m_SelectedEntity = entity;
		}
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Scene* m_Context = nullptr;
		Entity m_SelectedEntity;
	};
}