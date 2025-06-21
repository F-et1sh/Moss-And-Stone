#include "forpch.h"
#include "AnimationSystem.h"

void FE2D::AnimationSystem::Render() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto group = registry.group<AnimatorComponent>(entt::get<TransformComponent, SpriteComponent>);
	for (auto e : group) {
		Entity entity = { e, m_Scene };

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& animator = entity.GetComponent<AnimatorComponent>();
		auto& sprite = entity.GetComponent<SpriteComponent>();

		switch (animator.state) {
		case AnimatorComponent::State::PAUSE:
			break;
		
		case AnimatorComponent::State::PLAY:
			animator.time += m_Window->getDeltaTime();
			break;

		case AnimatorComponent::State::STOP:
			animator.time = 0;
			break;
		}

		auto chosen_id = GetNearAnimation(animator.animations, animator.current_direction);
		auto& chosen_animation = m_ResourceManager->GetResource(chosen_id);

		sprite.frame = chosen_animation.getFrameUV(animator.time);
		sprite.texture = chosen_animation.getTexture(*m_ResourceManager);
	}
}

#undef max

ResourceID<Animation> FE2D::AnimationSystem::GetNearAnimation(const std::vector<std::pair<vec2, ResourceID<Animation>>>& animation_points, vec2 current_direction) {
	if (animation_points.empty()) return ResourceID<Animation>(FE2D::UUID(0));

	float shortest_distance = std::numeric_limits<float>::max(); // some big value
	size_t current_animation = 0;

	for (size_t i = 0; i < animation_points.size(); i++) {

		float load_distance = dot(current_direction - animation_points[i].first,
								  current_direction - animation_points[i].first); // use dot() instead of distance()
		
		if (shortest_distance > load_distance) {
			shortest_distance = load_distance;
			current_animation = i;
		}
	}
	return animation_points[current_animation].second;
}
