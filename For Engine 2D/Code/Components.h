#pragma once
#include "UUID.h"
#include "Entity.h"
#include "ScriptableEntity.h"

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
                matrix = glm::translate(matrix, vec3(-origin*scale, 0));

                // apply rotation
                matrix = glm::rotate(matrix, glm::radians(rotation), vec3(0.0f, 0.0f, -1.0f));

                // reset origin
                matrix = glm::translate(matrix, vec3(origin*scale, 0));
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
        float velocity_dying = 1.025f;

        Entity entity_in;

        PhysicsComponent() = default;
        ~PhysicsComponent() = default;
        PhysicsComponent(const PhysicsComponent&) = default;
    };
    
    struct FOR_API AnimatorComponent {
        enum class State {
            PAUSE,
            PLAY,
            STOP,
        };

        std::vector<std::pair<vec2, ResourceID<Animation>>> animations;

        vec2 current_direction = vec2();
        float time = 0.0f;
        State state = State::STOP;

        AnimatorComponent() = default;
        ~AnimatorComponent() = default;
        AnimatorComponent(const AnimatorComponent&) = default;
    };

    struct FOR_API NativeScriptComponent {
        std::unique_ptr<ScriptableEntity> instance = nullptr;
        std::string script_name;
        
        NativeScriptComponent() = default;
        ~NativeScriptComponent() = default;

        NativeScriptComponent(const NativeScriptComponent& other) {
            if (other.instance)
                instance = std::move(other.instance->clone());
        }

        NativeScriptComponent& operator=(const NativeScriptComponent& other) {
            if (this != &other) {
                if (other.instance) instance = other.instance->clone();
                else instance.reset();
            }
            return *this;
        }

        NativeScriptComponent(NativeScriptComponent&&) noexcept = default;
        NativeScriptComponent& operator=(NativeScriptComponent&&) noexcept = default;
    };

    using AllComponents = std::variant<
        IDComponent, 
        TagComponent, 
        TransformComponent, 
        SpriteComponent, 
        RelationshipComponent, 
        CameraComponent, 
        PhysicsComponent, 
        AnimatorComponent, 
        NativeScriptComponent>;
}
