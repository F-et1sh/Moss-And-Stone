#pragma once
#include "Entity.h"

#include "ISystemBase.h"
#include "SpriteRendererSystem.h"
#include "PhysicsSystem.h"
#include "AnimationSystem.h"
#include "ScriptManagerSystem.h"
#include "HealthSystem.h"
#include "EffectsSystem.h"
#include "TilemapRendererSystem.h"

#include "RenderContext.h"
#include "Cursor.h"

#include "ProjectVariables.h"

namespace FE2D {
	class FOR_API Scene {
	public:
		Scene() = default;
		~Scene();

		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		void Release();
		void Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager, ProjectVariables& project_variables);

	public:

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = "");
		void DestroyEntity(Entity entity);

		void EmplaceEntity(Entity entity);
		void EmplaceEntity(FE2D::UUID uuid, Entity entity);

		inline entt::registry& getRegistry()noexcept { return m_Registry; }
		inline Window& getContextWindow() { return *m_Window; }
		inline RenderContext& getRenderContext() { return *m_RenderContext; }
		inline ResourceManager& getResourceManager() { return *m_ResourceManager; }
		inline ProjectVariables& getProjectVariables() { return *m_ProjectVariables; }

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

		template<typename T>
		inline T& getSystem() {
			for (auto* base : m_SystemsList) {
				if (auto* system = dynamic_cast<T*>(base))
					return *system;
			}
			FOR_ASSERT(false, "Failed to get system. There is no needed system");
		}

	private:
		template<typename T> requires std::is_base_of_v<ISystemBase, T>
		std::unique_ptr<T> CreateSystem() {
			std::unique_ptr<T> system = std::make_unique<T>();
			system->setContext(*m_Window, *m_RenderContext, *m_ResourceManager, *this, *m_ProjectVariables);
			m_SystemsList.emplace_back(system.get());
			return system;
		}

	private:
		std::unique_ptr<SpriteRendererSystem>	m_SpriteRendererSystem;
		std::unique_ptr<PhysicsSystem>			m_PhysicsSystem;
		std::unique_ptr<AnimationSystem>		m_AnimationSystem;
		std::unique_ptr<ScriptManagerSystem>	m_ScriptManagerSystem;
		std::unique_ptr<HealthSystem>			m_HealthSystem;
		std::unique_ptr<EffectsSystem>			m_EffectsSystem;
		std::unique_ptr<TilemapRendererSystem>  m_TilemapRendererSystem;

		std::vector<ISystemBase*> m_SystemsList; // this mustn't free data of systems

	private:
		std::unordered_map<UUID, Entity> m_EntityMap;

	private:
		Window* m_Window = nullptr;
		RenderContext* m_RenderContext = nullptr;
		ResourceManager* m_ResourceManager = nullptr;
		ProjectVariables* m_ProjectVariables = nullptr;

		Camera m_Camera;
		FE2D::UUID m_CameraEntityUUID = FE2D::UUID(0);

	private:
		size_t m_Index = 0; // scene index

	private:
		entt::registry m_Registry;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}