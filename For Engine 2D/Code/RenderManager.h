#pragma once
#include "Renderer.h"

namespace FE2D {
	class FOR_API RenderManager {
	public:
		RenderManager() = default;
		~RenderManager() = default;

		void Release();
		void Initialize(ResourceManager* resourceManager);
		
		void setCamera(Camera* camera)noexcept;

		//void HandleActor(AActor* actor);
		//void HandleComponent(UTransformComponent* transform, const UComponent* component, size_t hash_code);

		void Render();

	private:
		ResourceManager* m_ResourceManager = nullptr;
		Camera* m_Camera = nullptr;

	private:
		std::unordered_map<size_t, Renderer*> m_Renderers;
	};
}