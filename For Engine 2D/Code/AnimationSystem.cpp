#include "forpch.h"
#include "AnimationSystem.h"
#include "AnimationStateNodes.h"

void FE2D::AnimationSystem::Render() {
    entt::registry& registry = this->m_Scene->getRegistry();

    auto group = registry.group<AnimatorComponent>(entt::get<TransformComponent, SpriteComponent>);
    for (auto e : group) {
        Entity entity = { e, m_Scene };

        auto& transform = entity.GetComponent<TransformComponent>();
        auto& animator = entity.GetComponent<AnimatorComponent>();
        auto& sprite = entity.GetComponent<SpriteComponent>();

        double deltaTime = m_Window->getDeltaTime();
        animator.current_time += deltaTime;

        IStateNode* state = nullptr;
        for (const auto& s : animator.states) {
            if (s->name == animator.current_state) {
                state = s.get();
                break;
            }
        }
        if (!state) return;

        if (auto* blend = dynamic_cast<BlendTreeNode*>(state))
            blend->update_current_animation(animator.current_direction);

        for (const auto& transition : animator.transitions) {
            if (transition.from_state != animator.current_state) continue;

            if (state->looping && animator.current_time >= state->duration) {
                bool passed = true;

                for (const auto& cond : transition.conditions) {
                    const auto it = animator.parameters.find(cond.parameter_name);
                    if (it == animator.parameters.end()) {
                        passed = false;
                        break;
                    }

                    const auto& param = it->second;
                    float param_value = 0.0f;

                    if      (std::holds_alternative<bool >(param.value)) param_value = std::get<bool >(param.value) ? 1.0f : 0.0f;
                    else if (std::holds_alternative<float>(param.value)) param_value = std::get<float>(param.value);
                    else if (std::holds_alternative<int  >(param.value)) param_value = static_cast<float>(std::get<int>(param.value));

                    switch (cond.condition) {
                    case ConditionType::Equals:
                        if (param_value != cond.value) passed = false;
                        break;
                    case ConditionType::Greater:
                        if (param_value <= cond.value) passed = false;
                        break;
                    case ConditionType::Less:
                        if (param_value >= cond.value) passed = false;
                        break;
                    }

                    if (!passed) break;
                }

                if (passed) {
                    animator.current_state = transition.to_state;
                    animator.current_time = 0.0f;
                    break;
                }
            }
        }

        if (state->get_animation().uuid != FE2D::UUID(0)) { // state can't be nullptr here
			auto& animation = m_ResourceManager->GetResource(state->animation_id);
            sprite.texture = animation.getTexture(*m_ResourceManager);
            sprite.frame = animation.getFrameUV(animator.current_time);
		}
    }
}
