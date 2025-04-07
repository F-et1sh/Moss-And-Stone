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
		bool HasComponent()const {
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent() {
			FOR_ASSERT(HasComponent<T>(), "Entity does not have component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		void SetParent(Entity newParent) {
			if (!newParent.HasComponent<RelationshipComponent>())
				newParent.AddComponent<RelationshipComponent>();

			auto& relationship = GetComponent<RelationshipComponent>();

			if (HasParent()) {
				auto& oldParentRel = GetParent().GetComponent<RelationshipComponent>();
				oldParentRel.children.erase(
					std::remove(oldParentRel.children.begin(), oldParentRel.children.end(), m_EntityHandle),
					oldParentRel.children.end()
				);
			}

			relationship.parent = newParent;

			newParent.GetComponent<RelationshipComponent>().children.emplace_back(m_EntityHandle);
		}

		Entity GetParent() {
			FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
			return { m_Scene->m_Registry.get<RelationshipComponent>(m_EntityHandle).parent, m_Scene };
		}

		bool HasParent() {
			FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
			return m_Scene->m_Registry.get<RelationshipComponent>(m_EntityHandle).parent != entt::null;
		}

		void RemoveParent() {
			FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
			FOR_ASSERT(HasParent(), "Entity does not have parent");

			auto& relationship = GetComponent<RelationshipComponent>();

			Entity parentEntity = GetParent();
			auto& parentRel = parentEntity.GetComponent<RelationshipComponent>();
			parentRel.children.erase(
				std::remove(parentRel.children.begin(), parentRel.children.end(), m_EntityHandle),
				parentRel.children.end()
			);

			relationship.parent = entt::null;
		}

		void AddChild() {
			FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

			Entity child = this->m_Scene->CreateEntity("New Child");
			child.SetParent(*this);

			m_Scene->m_Registry.get<RelationshipComponent>(m_EntityHandle).children.push_back(child);
		}

		void AddChild(Entity child) {
			FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

			child.SetParent(*this);
		}

		std::vector<Entity> GetChildren() {
			FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

			const auto& source = m_Scene->m_Registry.get<RelationshipComponent>(m_EntityHandle).children;
			std::vector<Entity> children;
			children.reserve(source.size());

			for (entt::entity child : source) 
				children.emplace_back(child, m_Scene);

			return children;
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