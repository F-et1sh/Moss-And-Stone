#pragma once
#include "ComponentSpecial.h"
#include "SceneSerializer.h"
#include "UUID.h"

namespace FE2D {
    struct FOR_API IDComponent : ISerializableComponent {
        FE2D::UUID id;

        json Serialize()const override {
            json j;
            SceneSerializer::save_value(id.get(), j, "id");
            return j;
        }

        void Deserialize(const json& j) override {
            std::uint64_t temp_id;
            SceneSerializer::load_value(temp_id, j, "id");
            id = FE2D::UUID(temp_id);
        }

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
    };

    struct FOR_API TagComponent : ISerializableComponent {
		std::string tag;

        json Serialize()const override {
            json j;
            SceneSerializer::save_value(tag, j, "tag");
            return j;
        }

        void Deserialize(const json& j)override {
            SceneSerializer::load_value(tag, j, "tag");
        }

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : tag(tag) {}
    
    };
    
    struct FOR_API TransformComponent : IDrawableUIComponent, ISerializableComponent {
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

        void DrawUI(IMGUI& imgui)override {
            imgui.DragVector2("Position", position);
            ImGui::Spacing();
            imgui.DragInt("Layer", layer);
            ImGui::Spacing();
            imgui.CheckBox("Auto-Sotring", auto_sort);
            ImGui::Spacing();
            imgui.DragVector2("Scale", scale);
            ImGui::Spacing();
            imgui.DragFloat("Rotation", rotation);
            ImGui::Spacing();
            imgui.DragVector2("Origin", origin);

            imgui.TransformControl(*this);
        }

        json Serialize()const override {
            json j;
            SceneSerializer::save_vec2 (position , j, "position");
            SceneSerializer::save_value(layer    , j, "layer"   );
            SceneSerializer::save_value(auto_sort, j, "auto_sort");
            SceneSerializer::save_vec2 (scale    , j, "scale"   );
            SceneSerializer::save_value(rotation , j, "rotation");
            SceneSerializer::save_vec2 (origin   , j, "origin"  );
            return j;
        }

        void Deserialize(const json& j) override {
            SceneSerializer::load_vec2 (position , j, "position");
            SceneSerializer::load_value(layer    , j, "layer"   );
            SceneSerializer::load_value(auto_sort, j, "auto_sort");
            SceneSerializer::load_vec2 (scale    , j, "scale"   );
            SceneSerializer::load_value(rotation , j, "rotation");
            SceneSerializer::load_vec2 (origin   , j, "origin"  );
        }

        TransformComponent() = default;
        ~TransformComponent() = default;

        TransformComponent(const TransformComponent&) = default;
	};

    struct FOR_API SpriteComponent : IDrawableUIComponent, ISerializableComponent {

        vec4 texture_coords = vec4();

        size_t texture_index = 0;

        bool flip_x = false;
        bool flip_y = false;

        void DrawUI(IMGUI& imgui)override {
            auto texture_optional = imgui.SelectTexture();
            if (texture_optional.has_value()) {
                Texture& texture = texture_optional.value().second;
                
                texture_coords.x = 0;
                texture_coords.y = 0;
                texture_coords.z = texture.getSize().x;
                texture_coords.w = texture.getSize().y;

                texture_index = texture_optional.value().first;
            }

            imgui.CheckBox("Flip X", flip_x);
            imgui.CheckBox("Flip Y", flip_y);
        }

        json Serialize()const override {
            json j;
            SceneSerializer::save_vec4(texture_coords, j, "texture_coords");
            SceneSerializer::save_value(texture_index, j, "texture_index");
            SceneSerializer::save_value(flip_x, j, "flip_x");
            SceneSerializer::save_value(flip_y, j, "flip_y");
            return j;
        }

        void Deserialize(const json& j) override {
            SceneSerializer::load_vec4(texture_coords, j, "texture_coords");
            SceneSerializer::load_value(texture_index, j, "texture_index");
            SceneSerializer::load_value(flip_x, j, "flip_x");
            SceneSerializer::load_value(flip_y, j, "flip_y");
        }

        SpriteComponent() = default;
        ~SpriteComponent() = default;
        SpriteComponent(const SpriteComponent&) = default;
    };

    struct FOR_API RelationshipComponent : ISerializableComponent {
        UUID parent = UUID(0);
        std::vector<UUID> children;

        json Serialize() const override {
            json j;
            SceneSerializer::save_value(parent.get(), j, "parent");

            json children_array = json::array();
            for (const auto& child : children) {
                children_array.push_back(child.get());
            }
            j["children"] = std::move(children_array);

            return j;
        }

        void Deserialize(const json& j) override {
            if (j.contains("parent")) {
                std::uint64_t temp_id;
                SceneSerializer::load_value(temp_id, j, "parent");
                parent = UUID(temp_id);
            }

            if (j.contains("children")) {
                children.clear();
                children.reserve(j["children"].size());
                for (const auto& j_child : j["children"]) {
                    children.emplace_back(UUID(j_child.get<std::uint64_t>()));
                }
            }
        }

        RelationshipComponent() = default;
        ~RelationshipComponent() = default;

        RelationshipComponent(const RelationshipComponent&) = default;
    };

    struct FOR_API CameraComponent : IDrawableUIComponent, ISerializableComponent {
        vec4 clear_color = vec4();

        void DrawUI(IMGUI& imgui)override {
            ImGui::ColorEdit4("Clear Color", (float*)&clear_color);
        }

        json Serialize()const override {
            json j;
            SceneSerializer::save_vec4(clear_color, j, "clear_color");
            return j;
        }

        void Deserialize(const json& j) override {
            SceneSerializer::load_vec4(clear_color, j, "clear_color");
        }

        CameraComponent() = default;
        ~CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };
}
