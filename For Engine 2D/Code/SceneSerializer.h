#pragma once
#include "ComponentField.h"
#include "Scene.h"
#include "Prefab.h"

namespace FE2D {
    class FOR_API SceneSerializer {
    public:
        SceneSerializer(Scene* scene) : m_Scene(scene) {}

        bool Serialize(const std::filesystem::path& full_path);
        bool Deserialize(const std::filesystem::path& full_path);

    public:
        template<typename T> requires std::is_same_v<T, Prefab> || std::is_same_v<T, Entity>
        inline static json SerializeComponents(const T& entity) {
            json j;

            if (entity.HasComponent<IDComponent>()) {
                auto& component = entity.GetComponent<IDComponent>();

                json j_component;
                SceneSerializer::save_uuid(component.id, j_component, "id");

                j["IDComponent"] = j_component;
            }

            if (entity.HasComponent<TagComponent>()) {
                auto& component = entity.GetComponent<TagComponent>();

                json j_component;
                SceneSerializer::save_value(component.tag, j_component, "tag");

                j["TagComponent"] = j_component;
            }

            if (entity.HasComponent<TransformComponent>()) {
                auto& component = entity.GetComponent<TransformComponent>();

                json j_component;
                SceneSerializer::save_vec2(component.position, j_component, "position");
                SceneSerializer::save_value(component.layer, j_component, "layer");
                SceneSerializer::save_value(component.auto_sort, j_component, "auto_sort");
                SceneSerializer::save_vec2(component.scale, j_component, "scale");
                SceneSerializer::save_value(component.rotation, j_component, "rotation");
                SceneSerializer::save_vec2(component.origin, j_component, "origin");

                j["TransformComponent"] = j_component;
            }

            if (entity.HasComponent<SpriteComponent>()) {
                auto& component = entity.GetComponent<SpriteComponent>();

                json j_component;
                SceneSerializer::save_resource_id(component.texture, j_component, "texture");
                SceneSerializer::save_vec4(component.frame, j_component, "frame");
                SceneSerializer::save_value(component.flip_x, j_component, "flip_x");
                SceneSerializer::save_value(component.flip_y, j_component, "flip_y");

                j["SpriteComponent"] = j_component;
            }

            if (entity.HasComponent<RelationshipComponent>()) {
                auto& component = entity.GetComponent<RelationshipComponent>();

                json j_component;
                SceneSerializer::save_value(component.parent.get(), j_component, "parent");

                json children_array = json::array();
                for (const auto& child : component.children) {
                    children_array.push_back(child.get());
                }
                j_component["children"] = std::move(children_array);

                j["RelationshipComponent"] = j_component;
            }

            if (entity.HasComponent<CameraComponent>()) {
                auto& component = entity.GetComponent<CameraComponent>();

                json j_component;
                SceneSerializer::save_vec4(component.clear_color, j_component, "clear_color");

                j["CameraComponent"] = j_component;
            }

            if (entity.HasComponent<PhysicsComponent>()) {
                auto& component = entity.GetComponent<PhysicsComponent>();

                json j_component;
                SceneSerializer::save_vec2(component.position, j_component, "position");
                SceneSerializer::save_vec2(component.size, j_component, "size");
                SceneSerializer::save_value(component.bounce_factor, j_component, "bounce_factor");
                SceneSerializer::save_value(component.mass, j_component, "mass");
                SceneSerializer::save_value(component.is_trigger, j_component, "is_trigger");
                SceneSerializer::save_value(component.is_static, j_component, "is_static");
                SceneSerializer::save_value(component.velocity_dying, j_component, "velocity_dying");

                j["PhysicsComponent"] = j_component;
            }

            if (entity.HasComponent<AnimatorComponent>()) {
                auto& component = entity.GetComponent<AnimatorComponent>();

                json j_component;
                SceneSerializer::save_array(component.animations, j_component, "animations", [](const std::pair<vec2, ResourceID<Animation>>& e) -> json {
                    json j;
                    SceneSerializer::save_vec2(e.first, j, "coord");
                    SceneSerializer::save_resource_id(e.second, j, "animation");
                    return j;
                    });

                j["AnimatorComponent"] = j_component;
            }

            if (entity.HasComponent<NativeScriptComponent>()) {
                auto& component = entity.GetComponent<NativeScriptComponent>();

                json j_component;
                SceneSerializer::save_value(component.script_name, j_component, "script_name");
                SceneSerializer::save_value(component.instance ? component.instance->Serialize() : json(), j_component, "script_data");

                j["NativeScriptComponent"] = j_component;
            }

            return j;
        }

        inline static std::vector<AllComponents> DeserialzieComponents(const json& j) {
            std::vector<AllComponents> deserialized_components;

            if (j.contains("IDComponent")) {
                IDComponent component;
                
                const json& j_component = j["IDComponent"];
                SceneSerializer::load_uuid(component.id, j_component, "id");

                deserialized_components.emplace_back(std::move(component));
            }
            
            if (j.contains("TagComponent")) {
                TagComponent component;

                const json& j_component = j["TagComponent"];
                SceneSerializer::load_value(component.tag, j_component, "tag");

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("TransformComponent")) {
                TransformComponent component;

                const json& j_component = j["TransformComponent"];
                SceneSerializer::load_vec2(component.position, j_component, "position");
                SceneSerializer::load_value(component.layer, j_component, "layer");
                SceneSerializer::load_value(component.auto_sort, j_component, "auto_sort");
                SceneSerializer::load_vec2(component.scale, j_component, "scale");
                SceneSerializer::load_value(component.rotation, j_component, "rotation");
                SceneSerializer::load_vec2(component.origin, j_component, "origin");

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("SpriteComponent")) {
                SpriteComponent component;

                const json& j_component = j["SpriteComponent"];
                SceneSerializer::load_resource_id(component.texture, j_component, "texture");
                SceneSerializer::load_vec4(component.frame, j_component, "frame");
                SceneSerializer::load_value(component.flip_x, j_component, "flip_x");
                SceneSerializer::load_value(component.flip_y, j_component, "flip_y");

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("RelationshipComponent")) {
                RelationshipComponent component;

                const json& j_component = j["RelationshipComponent"];

                if (j_component.contains("parent")) {
                    std::uint64_t temp_id;
                    SceneSerializer::load_value(temp_id, j_component, "parent");
                    component.parent = UUID(temp_id);
                }

                if (j_component.contains("children")) {
                    component.children.clear();
                    component.children.reserve(j_component["children"].size());
                    for (const auto& j_child : j_component["children"]) {
                        component.children.emplace_back(UUID(j_child.get<std::uint64_t>()));
                    }
                }

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("CameraComponent")) {
                CameraComponent component;

                const json& j_component = j["CameraComponent"];
                SceneSerializer::load_vec4(component.clear_color, j_component, "clear_color");

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("PhysicsComponent")) {
                PhysicsComponent component;

                const json& j_component = j["PhysicsComponent"];
                SceneSerializer::load_vec2(component.position, j_component, "position");
                SceneSerializer::load_vec2(component.size, j_component, "size");
                SceneSerializer::load_value(component.bounce_factor, j_component, "bounce_factor");
                SceneSerializer::load_value(component.mass, j_component, "mass");
                SceneSerializer::load_value(component.is_trigger, j_component, "is_trigger");
                SceneSerializer::load_value(component.is_static, j_component, "is_static");
                SceneSerializer::load_value(component.velocity_dying, j_component, "velocity_dying");

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("AnimatorComponent")) {
                AnimatorComponent component;

                const json& j_component = j["AnimatorComponent"];
                SceneSerializer::load_array(component.animations, j_component, "animations",
                    [](const json& j) -> std::pair<vec2, ResourceID<Animation>> {
                        std::pair<vec2, ResourceID<Animation>> pair;
                        SceneSerializer::load_vec2(pair.first, j, "coord");
                        SceneSerializer::load_resource_id(pair.second, j, "animation");
                        return pair;
                    });

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("NativeScriptComponent")) {
                NativeScriptComponent component;

                const json& j_component = j["NativeScriptComponent"];
                SceneSerializer::load_value(component.script_name, j_component, "script_name");
                if (!ScriptFactory::Instance().GetRegisteredScripts().contains(component.script_name)) {
                    SAY("WARNING : Failed to deserialize a scriptable entity" <<
                        "\nFailed to find script name in the list. Using empty name" <<
                        "\nName : " << component.script_name.c_str());
                    component.script_name.clear();
                }

                component.instance = ScriptFactory::Instance().CreateScript(component.script_name);
                component.instance->Deserialize(j_component["script_data"]);

                deserialized_components.emplace_back(std::move(component));
            }

            return deserialized_components;
        }
        
    private:
        void SerializeSceneInfo(json& j);
        bool DeserializeSceneInfo(const json& j);
        
        Scene* m_Scene = nullptr;

	public:
        static void save_uuid(FE2D::UUID uuid, json& j, const std::string& name) {
            if (uuid.ToString().empty()) return;
            j[name] = uuid.ToString();
        }

        static void save_entity(Entity entity, json& j, const std::string& name) {
            if (!entity) return;
            j[name] = entity.GetComponent<IDComponent>().id.ToString();
        }

        template<typename T>
        static void save_component_field(ComponentField<T> field, json& j, const std::string& name) {
            if (!field.entity) return;
            j[name] = field.entity.GetUUID().ToString();
        }

        template<typename T> requires std::is_base_of_v<IResource, T>
        static void save_resource_id(ResourceID<T> id, json& j, const std::string& name) {
            j[name] = id.uuid.ToString();
        }

        template<typename T, typename = std::enable_if_t<!std::is_same_v<T, std::wstring> && !std::_Is_specialization_v<std::remove_cvref_t<T>, std::vector> && !std::is_pointer_v<T>>> requires (!std::is_base_of_v<IResource, T>)
        static void save_value(const T& value, json& j, const std::string& name) {
            j[name] = value;
        }

        template<typename Array, typename Func>
            requires (
                std::invocable<Func, typename Array::value_type>&&
                std::convertible_to<std::invoke_result_t<Func, typename Array::value_type>, json>
            )
            static void save_array(const Array& array, json& j, const std::string& name, Func&& func) {
            json j_array = json::array();
            for (const auto& e : array) {
                j_array.emplace_back(func(e));
            }
            j[name] = j_array;
        }

        static void save_value(const std::wstring& value, json& j, const std::string& name) {
            std::string load_value = FE2D::wstring_to_string(value);
            j[name] = load_value;
        }

        static void save_vec2(const vec2& value, json& j, const std::string& name) {
            j[name] = { value.x, value.y };
        }
        static void save_vec3(const vec3& value, json& j, const std::string& name) {
            j[name] = { value.x, value.y, value.z };
        }
        static void save_vec4(const vec4& value, json& j, const std::string& name) {
            j[name] = { value.x, value.y, value.z, value.w };
        }
    public:
        static void load_uuid(FE2D::UUID uuid, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            uuid = FE2D::UUID(j[name].get<std::string>());
        }

        static void load_entity(Entity& entity, const json& j, const std::string& name, Scene* scene) {
            if (!j.contains(name)) return;

            FE2D::UUID uuid = FE2D::UUID(j[name].get<std::string>());
            entity = scene->GetEntityByUUID(uuid);
        }

        template<typename T>
        static void load_component_field(ComponentField<T>& field, const json& j, const std::string& name, Scene* scene) {
            if (!j.contains(name)) return;

            FE2D::UUID uuid = FE2D::UUID(j[name].get<std::string>());
            field.entity = scene->GetEntityByUUID(uuid);
        }

        template<typename T> requires std::is_base_of_v<IResource, T>
        static void load_resource_id(ResourceID<T>& id, const json& j, const std::string& name) {
            if (!j.contains(name)) return;
            id.uuid = FE2D::UUID(j[name].get<std::string>());
        }

        template<typename T, typename = std::enable_if_t<!std::is_same_v<T, std::wstring> && !std::_Is_specialization_v<std::remove_cvref_t<T>, std::vector> && !std::is_pointer_v<T>>> requires (!std::is_base_of_v<IResource, T>)
        static void load_value(T& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;
            value = j[name].get<T>();
        }

        template<typename Array, typename Func>
            requires (
                std::invocable<Func, const json&>&&
                std::convertible_to<std::invoke_result_t<Func, const json&>, typename Array::value_type>
            )
            static void load_array(Array& array, const json& j, const std::string& name, Func&& func) {
            if (!j.contains(name)) return;
            if (!j[name].is_array()) return;

            const json& j_array = j[name];
            array.clear();

            if constexpr (requires(Array arr, typename Array::value_type v) { arr.emplace_back(v); }) {
                array.reserve(j_array.size());
                for (const json& e : j_array)
                    array.emplace_back(func(e));
            }
            else if constexpr (requires(Array arr, typename Array::value_type v) { arr.insert(v); }) {
                for (const json& e : j_array)
                    array.insert(func(e));
            }
            else FOR_ASSERT(true, "Container does not support emplace_back or insert");
        }

        static void load_value(std::wstring& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            std::string load_value = j[name].get<std::string>();
            value = FE2D::string_to_wstring(load_value);
        }

        static void load_vec2(vec2& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>() };
        }
        static void load_vec3(vec3& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>() };
        }
        static void load_vec4(vec4& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>(), load_value[3].get<float>() };
        }
	};
}
