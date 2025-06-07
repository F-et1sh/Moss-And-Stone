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

	auto group = registry.group<PlayerComponent>(entt::get<TransformComponent>);
	for (auto e : group) {
		Entity entity = { e, m_Scene };

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& player = entity.GetComponent<PlayerComponent>();

		if (!entity.HasComponent<CharacterAnimatorComponent>()) {
			SAY("WARNING : Entity has PlayerComponent but hasn't CharacterAnimatorComponent\nEntity : " << entity.GetComponent<TagComponent>().tag.c_str());
			continue;
		}

		auto& animator = entity.GetComponent<CharacterAnimatorComponent>();
		if (animator.animations.find("WalkDown") == animator.animations.end()) continue;
		if (animator.animations.find("LoliWalkDown") == animator.animations.end()) continue;

		auto& velocity = entity.GetComponent<VelocityComponent>();

		vec2 dir = vec2();
		constexpr float speed = 600;

		if (glfwGetKey(m_Window->reference(), GLFW_KEY_W)) dir += vec2( 0,  1);
		if (glfwGetKey(m_Window->reference(), GLFW_KEY_S)) dir += vec2( 0, -1);
		if (glfwGetKey(m_Window->reference(), GLFW_KEY_D)) dir += vec2( 1,  0);
		if (glfwGetKey(m_Window->reference(), GLFW_KEY_A)) dir += vec2(-1,  0);

		if (length(dir) != 0.0f) {
			dir = glm::normalize(dir);

			animator.current_animation = animator.animations.at("WalkDown");
		}
		else animator.current_animation = animator.animations.at("LoliWalkDown");

		velocity.velocity = dir * speed;
	}
}
