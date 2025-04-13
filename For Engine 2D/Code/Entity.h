#pragma once
#include "Scene.h"

#include <entt.hpp>

// Based on Hazel ( The Cherno ) - Apache License 2.0

namespace FE2D {
	class FOR_API Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		~Entity() = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			FOR_ASSERT(!HasComponent<T>(), "Entity already has component");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& GetComponent() {
			FOR_ASSERT(HasComponent<T>(), "Entity does not have component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		const T& GetComponent()const {
			FOR_ASSERT(HasComponent<T>(), "Entity does not have component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()const {
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent() {
			FOR_ASSERT(HasComponent<T>(), "Entity does not have component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		void SetParent(Entity newParent);
		Entity GetParent();
		bool HasParent();
		void RemoveParent();

		void AddChild();
		void AddChild(Entity child);
		std::vector<Entity> GetChildren();
		void RemoveChild(UUID child);

		UUID GetUUID()const {
			FOR_ASSERT(HasComponent<IDComponent>(), "Entity does not have IDComponent");
			return GetComponent<IDComponent>().id;
		}

		operator bool()const noexcept { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t()const noexcept { return (uint32_t)m_EntityHandle; }

		bool operator==(const Entity& other)const noexcept { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other)const noexcept { return !(*this == other); }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		friend class Scene;
	};
}