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

	auto group = registry.group<AnimatorComponent>(entt::get<TransformComponent>);
	for (auto e : group) {
		Entity entity = { e, m_Scene };

		if (!entity.HasComponent<SpriteComponent>()) {
			SAY("WARNING : Entity has AnimatorComponent but hasn't SpriteComponent\nEntity : " << entity.GetComponent<TagComponent>().tag.c_str());
			continue;
		}

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& animator  = entity.GetComponent<AnimatorComponent>(); 
		auto& sprite    = entity.GetComponent<SpriteComponent>();

		auto& current_animation = m_ResourceManager->GetResource(animator.current_animation);

		animator.time += m_Window->getDeltaTime();
		if (animator.time > current_animation.getMax())
			animator.time = 0;

		sprite.frame = current_animation.getFrame(animator.time);
		sprite.texture = current_animation.getTexture(*m_ResourceManager);
	}
}