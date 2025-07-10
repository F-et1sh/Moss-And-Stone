#include "forpch.h"
#include "EffectsSystem.h"

void FE2D::EffectsSystem::Update() {
	entt::registry& registry = this->m_Scene->getRegistry();

	this->HandleEffect<E_DamageComponent>(registry, [](Entity e, E_DamageComponent& c) {
		if (e.HasComponent<HealthComponent>()) {
			e.GetComponent<HealthComponent>().current_health -= c.damage;
		}
		e.RemoveComponent<E_DamageComponent>();
		});

	this->HandleEffect<E_FreezeComponent>(registry, [](Entity e, E_FreezeComponent& c) {
		if (e.HasComponent<HealthComponent>() && e.HasComponent<PhysicsComponent>()) {
			e.GetComponent<HealthComponent>().current_health -= 1;
			e.GetComponent<PhysicsComponent>().velocity = vec2(0);
			c.duration -= 0.5f;
		}
		if (c.duration <= 0)
			e.RemoveComponent<E_FreezeComponent>();
		});
}