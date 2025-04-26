#include "forpch.h"
#include "PlayerControllerSystem.h"

void FE2D::PlayerControllerSystem::Release()
{
}

void FE2D::PlayerControllerSystem::Initialize()
{
}

void FE2D::PlayerControllerSystem::Update() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto group = registry.group<PlayerComponent>(entt::get<TransformComponent, VelocityComponent>);
	for (auto e : group) {
		auto& transform = registry.get<TransformComponent>(e);
		auto& player = registry.get<PlayerComponent>(e);
		auto& velocity = registry.get<VelocityComponent>(e);

		vec2 dir = vec2();
		constexpr float speed = 600;

		if (glfwGetKey(m_Window->reference(), GLFW_KEY_W)) dir += vec2( 0,  1);
		if (glfwGetKey(m_Window->reference(), GLFW_KEY_S)) dir += vec2( 0, -1);
		if (glfwGetKey(m_Window->reference(), GLFW_KEY_D)) dir += vec2( 1,  0);
		if (glfwGetKey(m_Window->reference(), GLFW_KEY_A)) dir += vec2(-1,  0);

		if (length(dir) > 0.0f)
			dir = glm::normalize(dir);

		velocity.velocity = dir * speed;
	}
}
