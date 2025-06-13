#pragma once

namespace FE2D {
	/* forward declaration */
	class FOR_API Window;
	class FOR_API RenderContext;
	class FOR_API ResourceManager;
	class FOR_API Scene;
	class FOR_API IMGUI;

	class FOR_API ISystemBase {
	public:
		virtual ~ISystemBase() = default;

		void setContext(Window& window, RenderContext& render_context, ResourceManager& resource_manager, Scene& scene) {
			m_Window = &window;
			m_RenderContext = &render_context;
			m_ResourceManager = &resource_manager;
			m_Scene = &scene;
		}

		virtual void OnPropertiesPanel(IMGUI& imgui) {}

		virtual json Serialize()const { return json(); }
		virtual void Deserialize(const json& j) {}

	protected:
		Window* m_Window = nullptr;
		RenderContext* m_RenderContext = nullptr;
		ResourceManager* m_ResourceManager = nullptr;
		Scene* m_Scene = nullptr;
	};
}