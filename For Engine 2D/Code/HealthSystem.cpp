#include "forpch.h"
#include "HealthSystem.h"

void FE2D::HealthSystem::Update() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto view = registry.view<HealthComponent>();
	for (auto e : view) {
		Entity entity = { e, m_Scene };
		auto& health = entity.GetComponent<HealthComponent>();

		if (health.is_dead) continue; // entity is already dead
		
		if (health.current_health <= 0) {
			health.is_dead = true;
			
			if (entity.HasComponent<NativeScriptComponent>()) {
				auto instance = entity.GetComponent<NativeScriptComponent>().instance.get();
				if (instance) instance->OnDie();
			}
		}
	}
}