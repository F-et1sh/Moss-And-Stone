#pragma once
#include "Serializable.h"
#include "RenderManager.h"

#include "SpriteRendererSystem.h"

namespace FE2D {
	class FOR_API Entity;

	class FOR_API Scene {
	public:
		Scene() = default;
		~Scene() = default;

		void Release();
		void Initialize(Window* window, ResourceManager* resourceManager);

	public:

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);

		inline entt::registry& getRegistry()noexcept { return m_Registry; }

	public:

		void StartGame();
		void EndGame();

		void UpdateGame();
		void RenderGame();

	private:
		std::vector<std::unique_ptr<SystemBase>> m_Systems;

	private:
		entt::registry m_Registry;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}