#pragma once
#include "SceneSerializer.h"
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

		void EmplaceEntity(Entity entity);

		inline entt::registry& getRegistry()noexcept { return m_Registry; }

	public:

		void Update();
		void Render();

		void RenderPickable(Shader& shader, UniformBuffer& ubo);

	public:
		inline Entity& GetEntityByUUID(UUID uuid) { return m_EntityMap[uuid]; }
		
		inline std::unordered_map<UUID, Entity>& getEntityMap()noexcept { return m_EntityMap; }
		inline size_t getIndex()const noexcept { return m_Index; }

	private:
		std::vector<std::unique_ptr<SystemBase>> m_Systems;

	private:
		std::unordered_map<UUID, Entity> m_EntityMap;

	public:
		inline void setCamera(Camera* editor_camera)noexcept {
			m_EditorCamera = editor_camera;
		}
		inline Camera& getCamera()noexcept {
			if (m_EditorCamera)
				return *m_EditorCamera;
			else
				return m_Camera;
		}

	private:
		Camera* m_EditorCamera = nullptr;
		Camera m_Camera;

	private:
		size_t m_Index = 0;

	private:
		entt::registry m_Registry;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}