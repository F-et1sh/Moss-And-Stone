#pragma once

namespace FE2D {
	/* forward declarations */
	class FOR_API Scene;
	class FOR_API Shader;
	class FOR_API UniformBuffer;
	class FOR_API RenderContext;

	class FOR_API SystemBase {
	public:
		virtual ~SystemBase() = default;

		void setContext(Window& window, RenderContext& render_context, ResourceManager& resource_manager, Scene& scene) {
			m_Window = &window;
			m_RenderContext = &render_context;
			m_ResourceManager = &resource_manager;
			m_Scene = &scene;
		}

	protected:
		Window* m_Window = nullptr;
		RenderContext* m_RenderContext = nullptr;
		ResourceManager* m_ResourceManager = nullptr;
		Scene* m_Scene = nullptr;
	};
}