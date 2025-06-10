#pragma once
#include "SceneSerializer.h"
#include "UUID.h"

#include "Animation.h"

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

        // Flag for Auto Sprite Sorting
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

    struct FOR_API PlayerComponent {
        float a = 0.0f; // temp value to compile the project

        PlayerComponent() = default;
        ~PlayerComponent() = default;
        PlayerComponent(const PlayerComponent&) = default;
    };

    struct FOR_API VelocityComponent {
        vec2 velocity = vec2();

        VelocityComponent() = default;
        ~VelocityComponent() = default;
        VelocityComponent(const VelocityComponent&) = default;
    };

    struct FOR_API ColliderComponent {
        // position of the collider is local
        // ensure that you're using TransformComponent::position with ColliderComponent::position
        vec2 position = vec2(0, 0);
        vec2 size = vec2(50, 50);

        ColliderComponent() = default;
        ~ColliderComponent() = default;
        ColliderComponent(const ColliderComponent&) = default;
    };
    
    struct FOR_API CharacterAnimatorComponent {
        ResourceID<Animation> current_animation;
        float time = 0.0f;
        bool playing = false;
        bool loop = true;

        std::unordered_map<std::string, ResourceID<Animation>> animations;
    };
}
