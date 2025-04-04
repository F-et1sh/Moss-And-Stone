#pragma once

namespace FE2D {
    struct FOR_API TagComponent {
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : tag(tag) {}
    };
    
    struct FOR_API TransformComponent {
        vec2 position = vec2();

        int layer = 1;

        // Flag for Auto Sprite Sorting
        bool auto_sort = false;

        vec2 scale = vec2(1.0f, 1.0f);

        float rotation = 0.0f;
        vec2  origin = vec2();

        operator mat4() {
            mat4 matrix = mat4(1.0f);

            // scaling
            {
                matrix = glm::scale(matrix, vec3(scale, 1.0f));
            }

            // rotation
            {
                // set origin
                matrix = glm::translate(matrix, vec3(-origin*scale, 0));

                // apply rotation
                matrix = glm::rotate(matrix, glm::radians(rotation), vec3(0.0f, 0.0f, 1.0f));

                // reset origin
                matrix = glm::translate(matrix, vec3(origin*scale, 0));
            }

            // translation
            {
                matrix = glm::translate(matrix, vec3(position,                // position
                    (auto_sort ? (-position.y * std::abs(scale.y) / 2) : 0) + // auto sorting
                    layer));                                                  // layer
            }

            return matrix;
        }

        TransformComponent() = default;
        ~TransformComponent() = default;

        TransformComponent(const TransformComponent&) = default;
	};

    struct FOR_API SpriteComponent {

        vec4 m_TextureCoords = vec4();

        size_t m_TextureIndex = 0;

        bool m_FlipX = false;
        bool m_FlipY = false;

        SpriteComponent() = default;
        ~SpriteComponent() = default;
        SpriteComponent(const SpriteComponent&) = default;
    };

    struct FOR_API RelationshipComponent {
        entt::entity parent{ entt::null };
        std::vector<entt::entity> children;

        RelationshipComponent() = default;
        ~RelationshipComponent() = default;

        RelationshipComponent(const RelationshipComponent&) = default;
    };
}
