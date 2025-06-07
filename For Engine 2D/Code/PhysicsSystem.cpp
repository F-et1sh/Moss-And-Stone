#include "forpch.h"
#include "PhysicsSystem.h"

void FE2D::PhysicsSystem::Release()
{
}

void FE2D::PhysicsSystem::Initialize()
{
}

void FE2D::PhysicsSystem::Update() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto group = registry.group<VelocityComponent>(entt::get<TransformComponent>);
	for (auto e : group) {
		auto& transform = registry.get<TransformComponent>(e);
		auto& velocity = registry.get<VelocityComponent>(e);
		
		const double deltaTime = m_Window->getDeltaTime();

		transform.position += velocity.velocity * vec2(deltaTime);
	}
}
