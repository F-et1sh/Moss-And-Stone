#pragma once

namespace FE2D {
	/* forward declarations */
	class FOR_API Scene;
	class FOR_API Shader;
	class FOR_API UniformBuffer;

	class FOR_API SystemBase {
	public:
		virtual ~SystemBase() = default;

		void setContext(Window* window, ResourceManager* resourceManager, Scene* scene) {
			m_Window = window;
			m_ResourceManager = resourceManager;
			m_Scene = scene;
		}

		virtual void Release() = 0;
		virtual void Initialize() = 0;

		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void RenderPickable(Shader& shader, UniformBuffer& ubo) {}

	protected:
		Window* m_Window = nullptr;
		ResourceManager* m_ResourceManager = nullptr;
		Scene* m_Scene = nullptr;
	};
}