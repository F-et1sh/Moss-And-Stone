#pragma once

namespace FE2D {
	class FOR_API Prefab : public IResource {
	public:
		Prefab() = default;
		~Prefab() = default;

		bool LoadFromFile(const std::filesystem::path& file_path) override;
		void UplopadToFile(const std::filesystem::path& file_path) const override;

		void OnEditorDraw(IMGUI& imgui) override;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			FOR_ASSERT(!HasComponent<T>(), "Prefab already has component");
			T component{ std::forward<Args>(args)... };
			m_Components.emplace_back(std::move(component));

			return std::get<T>(m_Components.back());
		}

		template<typename T>
		T& GetComponent() {
			FOR_ASSERT(HasComponent<T>(), "Prefab does not have component");
			for (auto& comp : m_Components)
				if (auto* ptr = std::get_if<T>(&comp))
					return *ptr;
		}

		template<typename T>
		const T& GetComponent()const {
			FOR_ASSERT(HasComponent<T>(), "Prefab does not have component");
			for (const auto& comp : m_Components)
				if (const auto* ptr = std::get_if<T>(&comp))
					return *ptr;
		}

		template<typename T>
		bool HasComponent() const {
			for (const auto& component : m_Components)
				if (std::holds_alternative<T>(component)) 
					return true;
			return false;
		}

		template<typename T>
		void RemoveComponent() {
			FOR_ASSERT(HasComponent<T>(), "Prefab does not have component");

			auto it = std::find_if(m_Components.begin(), m_Components.end(),
				[](const AllComponents& c) { return std::holds_alternative<T>(c); });

			if (it != m_Components.end()) m_Components.erase(it);
		}

	private:
		std::vector<AllComponents> m_Components;
	};
}
