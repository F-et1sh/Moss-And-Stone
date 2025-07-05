#pragma once
#include "UUID.h"
#include "Entity.h"
#include "ScriptableEntity.h"
#include "AnimationStateNodes.h"

namespace FE2D {
    struct FOR_API IDComponent {
        FE2D::UUID id;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(FE2D::UUID id) : id(id) {}
    };

    struct FOR_API TagComponent {
        std::string tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : tag(tag) {}
    };

    struct FOR_API TransformComponent {
        vec2 position = vec2();

        int layer = 0;

        // flag for auto sprite sorting by Y-axis
        bool auto_sort = false;

        vec2 scale = vec2(1.0f, 1.0f);

        float rotation = 0.0f;
        vec2  origin = vec2();

        inline operator mat4()const noexcept {
            mat4 matrix = mat4(1.0f);

            // translation
            {
                matrix = glm::translate(matrix, vec3(position,                // position
                    (auto_sort ? (-position.y * std::abs(scale.y) / 2) : 0) + // auto-sorting
                    layer));                                                  // layer
            }

            // rotation
            {
                // set origin
                matrix = glm::translate(matrix, vec3(-origin * scale, 0));

                // apply rotation
                matrix = glm::rotate(matrix, glm::radians(rotation), vec3(0.0f, 0.0f, -1.0f));

                // reset origin
                matrix = glm::translate(matrix, vec3(origin * scale, 0));
            }

            // scaling
            {
                matrix = glm::scale(matrix, vec3(scale, 1.0f));
            }

            return matrix;
        }

        TransformComponent() = default;
        ~TransformComponent() = default;

        TransformComponent(const TransformComponent&) = default;
    };

    struct FOR_API SpriteComponent {
        ResourceID<Texture> texture;

        vec4 frame = vec4();

        bool flip_x = false;
        bool flip_y = false;

        SpriteComponent() = default;
        ~SpriteComponent() = default;
        SpriteComponent(const SpriteComponent&) = default;
    };

    struct FOR_API RelationshipComponent {
        FE2D::UUID parent = FE2D::UUID(0);
        std::vector<FE2D::UUID> children;

        RelationshipComponent() = default;
        ~RelationshipComponent() = default;

        RelationshipComponent(const RelationshipComponent&) = default;
    };

    struct FOR_API CameraComponent {
        vec4 clear_color = vec4();

        CameraComponent() = default;
        ~CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct FOR_API PhysicsComponent {
        vec2 position = vec2();
        vec2 size = vec2(50, 50);

        // 0 - no bounce
        // 1 - full bounce
        float bounce_factor = 0.0f;

        float mass = 1.0f;

        bool is_trigger = false;
        bool is_static = false;

        vec2 velocity = vec2();

        std::vector<Entity> entities_in;

        PhysicsComponent() = default;
        ~PhysicsComponent() = default;
        PhysicsComponent(const PhysicsComponent&) = default;
    };

    struct FOR_API AnimatorComponent {
        size_t current_state = 0;
        float current_time = 0.0f;

        std::vector<std::unique_ptr<IStateNode>> states;
        std::vector<AnimationTransition> transitions;
        // name | parameter
        std::unordered_map<std::string, AnimationParameter> parameters;

        inline void setBoolean(const std::string& name, bool value) { this->set_value(name, value); }
        inline void setFloat(const std::string& name, float value) { this->set_value(name, value); }
        inline void setInteger(const std::string& name, int value) { this->set_value(name, value); }
        inline void callTrigger(const std::string& name, float time = 0.5f) {
            auto it = parameters.find(name);
            if (it == parameters.end()) return;

            if (std::holds_alternative<Trigger>(it->second.value))
                std::get<Trigger>(it->second.value).trigger(time);
        }

        AnimatorComponent() = default;
        ~AnimatorComponent() = default;

        AnimatorComponent(const AnimatorComponent& other) {
            current_state = other.current_state;
            current_time = other.current_time;
            transitions = other.transitions;
            parameters = other.parameters;

            states.reserve(other.states.size());
            for (const auto& state : other.states) {
				if (state) states.emplace_back(state->clone());
            }
        }
        AnimatorComponent& operator=(const AnimatorComponent& other) {
            if (this == &other) return *this;

            states.clear();

            current_state = other.current_state;
            current_time = other.current_time;
            transitions = other.transitions;
            parameters = other.parameters;

            states.reserve(other.states.size());
            for (const auto& state : other.states) {
                if (state) states.emplace_back(state->clone());
            }

            return *this;
        }

        AnimatorComponent(AnimatorComponent&&) noexcept = default;
        AnimatorComponent& operator=(AnimatorComponent&&) noexcept = default;

    private:
        template<typename T> requires (std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, int> || std::is_same_v<T, Trigger>)
        inline void set_value(const std::string& n, T v) {
            auto it = parameters.find(n);
            if (it == parameters.end()) return;

            if (std::holds_alternative<T>(it->second.value))
                it->second.value = v;
        }
    };

    struct FOR_API NativeScriptComponent {
        std::unique_ptr<ScriptableEntity> instance = nullptr;
        std::string script_name;

        NativeScriptComponent() = default;
        ~NativeScriptComponent() = default;

        NativeScriptComponent(const NativeScriptComponent& other) {
            if (other.instance) {
                instance = std::move(other.instance->clone());
                script_name = other.script_name;
            }
        }

        NativeScriptComponent& operator=(const NativeScriptComponent& other) {
            if (this != &other) {
                if (other.instance) {
                    instance = std::move(other.instance->clone());
                    script_name = other.script_name;
                }
                else {
                    instance.reset();
                    script_name.clear();
                }
            }
            return *this;
        }

        NativeScriptComponent(NativeScriptComponent&&) noexcept = default;
        NativeScriptComponent& operator=(NativeScriptComponent&&) noexcept = default;
    };

    template<typename... Components>
    struct FOR_API ComponentGroup 
    {
    };

    using AllComponentsGroup = ComponentGroup<
        IDComponent,
        TagComponent,
        TransformComponent,
        SpriteComponent,
        RelationshipComponent,
        CameraComponent,
        PhysicsComponent,
        AnimatorComponent,
        NativeScriptComponent>;

    template<typename Group>
    struct FOR_API ComponentsHelper;

    template<typename... Components>
    struct FOR_API ComponentsHelper<ComponentGroup<Components...>> {
        using variant = std::variant<Components...>;

        inline static void collect_from_entity(Entity entity, std::vector<variant>& out) {
            CollectFromEntity<Components...>(entity, out);
        }

    private:
        template<typename T, typename... Rest>
        inline static void CollectFromEntity(Entity entity, std::vector<variant>& out) {
            if (entity.HasComponent<T>())
                out.emplace_back(entity.GetComponent<T>());
            if constexpr (sizeof...(Rest) > 0)
                CollectFromEntity<Rest...>(entity, out);
        }
    };

    using FOR_COMPONENTS_HELPER = ComponentsHelper<AllComponentsGroup>;
    using ComponentsVariant = FOR_COMPONENTS_HELPER::variant;
}
