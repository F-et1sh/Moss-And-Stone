#include "forpch.h"
#include "AnimationSystem.h"
#include "AnimationStateNodes.h"

void FE2D::AnimationSystem::Render() {
    entt::registry& registry = this->m_Scene->getRegistry();

    auto group = registry.group<AnimatorComponent>(entt::get<TransformComponent, SpriteComponent>);
    for (auto e : group) {
        Entity entity = { e, m_Scene };

        auto& transform = entity.GetComponent<TransformComponent>();
        auto& animator  = entity.GetComponent<AnimatorComponent>();
        auto& sprite    = entity.GetComponent<SpriteComponent>();

        double deltaTime = m_Window->getDeltaTime();
        animator.current_time += deltaTime;

        IStateNode* state = nullptr;
        for (size_t i = 0; i < animator.states.size(); i++) {
            if (i == animator.current_state) {
                state = animator.states[i].get();
                break;
            }
        }
        if (!state || state->animation_id.uuid == FE2D::UUID(0)) continue;

        if (auto blend = dynamic_cast<BlendTreeNode*>(state)) {
            auto it_x = std::find_if(animator.parameters.begin(), animator.parameters.end(), [blend](const std::pair<std::string, AnimationParameter>& e) { return e.first == blend->parameter_name_x; });
            auto it_y = std::find_if(animator.parameters.begin(), animator.parameters.end(), [blend](const std::pair<std::string, AnimationParameter>& e) { return e.first == blend->parameter_name_y; });
            
            if (it_x != animator.parameters.end() &&
                it_y != animator.parameters.end()) {

                vec2 direction = vec2(std::get<float>(it_x->second.value), 
                                      std::get<float>(it_y->second.value));

                blend->update_current_animation(direction);
            }
        }

        auto& animation = m_ResourceManager->GetResource(state->animation_id); // there is no error
        sprite.texture = animation.getTexture(*m_ResourceManager);
        sprite.frame = animation.getFrameUV(animator.current_time);

        for (const auto& transition : animator.transitions) {
            if (transition.from_state != animator.current_state) continue;

            if (state->looping || animator.current_time >= animation.getDuration()) {
                bool passed = true; // turn on the next animation

                for (const auto& cond : transition.conditions) { // look though all transition conditions
                    const auto it = animator.parameters.find(cond.parameter_name);
                    if (it == animator.parameters.end()) {
                        passed = false; // parameter wasn't found
                        break;
                    }

                    auto& param = it->second; // parameter of AnimatorComponent
                    float param_value = 0.0f;

                    if      (std::holds_alternative<bool   >(param.value)) param_value = std::get<bool >(param.value) ? 1.0f : 0.0f;
                    else if (std::holds_alternative<float  >(param.value)) param_value = std::get<float>(param.value);
                    else if (std::holds_alternative<int    >(param.value)) param_value = static_cast<float>(std::get<int>(param.value));
                    else if (std::holds_alternative<Trigger>(param.value)) {
                        Trigger& trigger = std::get<Trigger>(param.value);
                        param_value = trigger.is_triggered();
                        trigger.reset();
                    }

                    switch (cond.condition_type) {
                        case ConditionType::Equals : if (param_value != cond.value) passed = false; break;
                        case ConditionType::Greater: if (param_value <= cond.value) passed = false; break;
                        case ConditionType::Less   : if (param_value >= cond.value) passed = false; break;
                    }

                    if (!passed) break; // transition condition not met
                }

                if (passed) {
                    animator.current_state = transition.to_state;
                    animator.current_time = 0.0f;
                    break;
                }
            }
        }
    }
}
