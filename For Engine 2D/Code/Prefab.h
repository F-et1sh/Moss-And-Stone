#pragma once
#include "Components.h"
#include "Entity.h"

namespace FE2D {
	struct FOR_API PrefabEntity {
		std::vector<ComponentsVariant> m_Components;

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args) {
			FOR_ASSERT(!HasComponent<T>(), "Prefab already has component");
			T component{ std::forward<Args>(args)... };
			m_Components.emplace_back(std::move(component));

			return std::get<T>(m_Components.back());
		}

		template<typename T>
		inline T& GetComponent() {
			FOR_ASSERT(HasComponent<T>(), "Prefab does not have component");
			for (auto& comp : m_Components)
				if (auto* ptr = std::get_if<T>(&comp))
					return *ptr;
		}

		template<typename T>
		inline const T& GetComponent()const {
			FOR_ASSERT(HasComponent<T>(), "Prefab does not have component");
			for (const auto& comp : m_Components)
				if (const auto* ptr = std::get_if<T>(&comp))
					return *ptr;
		}

		template<typename T>
		inline bool HasComponent() const {
			for (const auto& component : m_Components)
				if (std::holds_alternative<T>(component))
					return true;
			return false;
		}

		template<typename T>
		inline void RemoveComponent() {
			FOR_ASSERT(HasComponent<T>(), "Prefab does not have component");

			auto it = std::find_if(m_Components.begin(), m_Components.end(),
				[](const ComponentsVariant& c) { return std::holds_alternative<T>(c); });

			if (it != m_Components.end()) m_Components.erase(it);
		}

		PrefabEntity() = default;
		~PrefabEntity() = default;

		PrefabEntity(std::vector<ComponentsVariant>& components) : m_Components(std::move(components)) {}
	};

	class FOR_API Prefab : public IResource {
	public:
		Prefab() = default;
		~Prefab() = default;
		
		explicit Prefab(Entity entity);

		bool LoadFromFile(const std::filesystem::path& file_path) override;
		void UploadToFile(const std::filesystem::path& file_path) const override;

		// this function will spawn the 'main' entity and its children on the scene
		// and return the 'main' entity
		inline Entity CreateEntity(Scene& scene);

		inline void OnEditorDraw(IMGUI& imgui) override;

		inline PrefabEntity& main_entity() {
			if (m_Entities.empty()) m_Entities.push_back({});
			return m_Entities[0]; // the first entity is the 'main'
		}

	private:
		// stores the 'main' entity and its children
		std::vector<PrefabEntity> m_Entities;
	};
}
