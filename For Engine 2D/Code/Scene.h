#pragma once
#include "RenderContext.h"

#include "SceneSerializer.h"

/* Included Systems */
#include "SpriteRendererSystem.h"
#include "PlayerControllerSystem.h"
#include "PhysicsSystem.h"
#include "AnimationSystem.h" 

namespace FE2D {
	class FOR_API Entity;

	class FOR_API Scene {
	public:
		Scene() = default;
		~Scene() = default;

		void Release();
		void Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager);

	public:

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = "");
		void DestroyEntity(Entity entity);

		void EmplaceEntity(Entity entity);

		inline entt::registry& getRegistry()noexcept { return m_Registry; }

	public:

		void End();
		void Start();

		void Update();
		void Render();

		void RenderPickable(RenderContext& render_context, MousePicker& mouse_picker);

	public:
		inline Entity& GetEntityByUUID(UUID uuid) { return m_EntityMap[uuid]; }
		
		inline std::unordered_map<UUID, Entity>& getEntityMap()noexcept { return m_EntityMap; }
		inline size_t getIndex()const noexcept { return m_Index; }

	private:
		template<typename T> requires std::is_base_of_v<SystemBase, T>
		std::unique_ptr<T> CreateSystem() {
			std::unique_ptr<T> system = std::make_unique<T>();
			system->setContext(*m_Window, *m_RenderContext, *m_ResourceManager, *this);
			system->Initialize();
			return system;
		}

	private:
		std::unique_ptr<SpriteRendererSystem> m_SpriteRendererSystem;
		std::unique_ptr<PlayerControllerSystem> m_PlayerControllerSystem;
		std::unique_ptr<PhysicsSystem> m_PhysicsSystem;
		std::unique_ptr<AnimationSystem> m_AnimationSystem;

	private:
		std::unordered_map<UUID, Entity> m_EntityMap;

	private:
		Window* m_Window = nullptr;
		RenderContext* m_RenderContext = nullptr;
		ResourceManager* m_ResourceManager = nullptr;
		Camera m_Camera;

	private:
		size_t m_Index = 0; // scene index

	private:
		entt::registry m_Registry;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}