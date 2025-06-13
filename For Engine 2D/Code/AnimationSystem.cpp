#include "forpch.h"
#include "AnimationSystem.h"

void FE2D::AnimationSystem::Release() {

}

void FE2D::AnimationSystem::Initialize() {

}

void FE2D::AnimationSystem::Render() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto group = registry.group<CharacterAnimatorComponent>(entt::get<TransformComponent>);
	for (auto e : group) {
		Entity entity = { e, m_Scene };

		if (!entity.HasComponent<SpriteComponent>()) {
			SAY("WARNING : Entity has CharacterAnimatorComponent but hasn't SpriteComponent\nEntity : " << entity.GetComponent<TagComponent>().tag.c_str());
			continue;
		}

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& animator = entity.GetComponent<CharacterAnimatorComponent>();
		auto& sprite = entity.GetComponent<SpriteComponent>();

		if (animator.current_animation.uuid == FE2D::UUID(0))
			continue;

		auto& current_animation = m_ResourceManager->GetResource(animator.current_animation);

		sprite.frame = current_animation.getFrameUV(animator.time);
		sprite.texture = current_animation.getTexture(*m_ResourceManager);
	}
}