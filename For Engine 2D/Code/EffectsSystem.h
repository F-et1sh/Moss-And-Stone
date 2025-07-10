#pragma once

namespace FE2D {
	class FOR_API EffectsSystem : public ISystemBase {
	public:
		EffectsSystem() = default;
		~EffectsSystem() = default;

		void Update();

	private:
		template<typename T>
		void HandleEffect(entt::registry& registry, const std::function<void(Entity, T&)>& func) {
			auto view = registry.view<T>();
			for (auto e : view) {
				Entity entity = { e, m_Scene };
				auto& effect = entity.GetComponent<T>();
				func(entity, effect);
			}
		}
	};
}