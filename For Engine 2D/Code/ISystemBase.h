#pragma once

namespace FE2D {
	/* forward declaration */
	class Window;
	class RenderContext;
	class ResourceManager;
	class Scene;
	class IMGUI;
	class ProjectVariables;

	class FOR_API ISystemBase {
	public:
		virtual ~ISystemBase() = default;

		void setContext(Window& window, RenderContext& render_context, ResourceManager& resource_manager, Scene& scene, ProjectVariables& project_variables) {
			m_Window = &window;
			m_RenderContext = &render_context;
			m_ResourceManager = &resource_manager;
			m_Scene = &scene;
			m_ProjectVariables = &project_variables;
		}

		virtual void OnPropertiesPanel(IMGUI& imgui) {}

		virtual json Serialize()const { return json(); }
		virtual void Deserialize(const json& j) {}

	protected:
		Window* m_Window = nullptr;
		RenderContext* m_RenderContext = nullptr;
		ResourceManager* m_ResourceManager = nullptr;
		Scene* m_Scene = nullptr;
		ProjectVariables* m_ProjectVariables = nullptr;
	};
}