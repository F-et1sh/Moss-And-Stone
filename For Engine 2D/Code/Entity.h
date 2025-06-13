#pragma once
#include <entt.hpp>

// Based on Hazel ( The Cherno ) - Apache License 2.0

namespace FE2D {
	class FOR_API Scene; // forward declaration
	
	class FOR_API Entity {
	public:
		Entity() = default;
		~Entity() = default;

		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args) {
			FOR_ASSERT(!HasComponent<T>(), "Entity already has component");
			T& component = this->GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		inline T& GetComponent() {
			FOR_ASSERT(HasComponent<T>(), "Entity does not have component");
			return this->GetRegistry().get<T>(m_EntityHandle);
		}

		template<typename T>
		inline const T& GetComponent()const {
			FOR_ASSERT(HasComponent<T>(), "Entity does not have component");
			return this->GetRegistry().get<T>(m_EntityHandle);
		}

		template<typename T>
		inline bool HasComponent()const {
			return this->GetRegistry().all_of<T>(m_EntityHandle);
		}

		template<typename T>
		inline void RemoveComponent() {
			FOR_ASSERT(HasComponent<T>(), "Entity does not have component");
			this->GetRegistry().remove<T>(m_EntityHandle);
		}

		void SetParent(Entity newParent);
		Entity GetParent();
		bool HasParent();
		void RemoveParent();

		void AddChild();
		void AddChild(Entity child);
		std::vector<Entity> GetChildren();
		void RemoveChild(UUID child);

		inline UUID GetUUID()const {
			FOR_ASSERT(HasComponent<IDComponent>(), "Entity does not have IDComponent");
			return GetComponent<IDComponent>().id;
		}

		operator bool()const noexcept { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t()const noexcept { return (uint32_t)m_EntityHandle; }

		bool operator==(const Entity& other)const noexcept { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other)const noexcept { return !(*this == other); }
	public:
		inline mat4 GetGlobalTransform() {
			auto& transform = GetComponent<TransformComponent>();
			mat4 global_transform = mat4();
			if (HasParent()) global_transform = GetGlobalTransform(GetParent()) * static_cast<mat4>(transform);
			else			 global_transform = static_cast<mat4>(transform);
			return global_transform;
		}
		inline static mat4 GetGlobalTransform(Entity entity) {
			auto& transform = entity.GetComponent<TransformComponent>();
			mat4 global_transform = mat4();
			if (entity.HasParent()) global_transform = GetGlobalTransform(entity.GetParent()) * static_cast<mat4>(transform);
			else					global_transform = static_cast<mat4>(transform);
			return global_transform;
		}

	private:
		inline entt::registry& GetRegistry()const noexcept;
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		friend class Scene;
	};
}