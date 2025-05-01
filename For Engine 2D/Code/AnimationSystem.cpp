#include "forpch.h"
#include "AnimationSystem.h"

void FE2D::AnimationSystem::Release()
{
}

void FE2D::AnimationSystem::Initialize()
{
}

void FE2D::AnimationSystem::Update() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto group = registry.group<AnimatorComponent>(entt::get<TransformComponent, SpriteComponent>);
	for (auto e : group) {
		auto& transform = registry.get<TransformComponent>(e);
		auto& animator = registry.get<AnimatorComponent>(e);
		auto& sprite = registry.get<SpriteComponent>(e);
		
		if (animator.animations.empty())
			continue;
		
		const size_t anim_index = animator.animations[animator.current_animation];
		Animation& animation = m_ResourceManager->getResource<Animation>(anim_index);

		sprite.texture_index = animation.getFrameTexture((size_t)animator.time);

		animator.time += m_Window->getDeltaTime();
	}
}

