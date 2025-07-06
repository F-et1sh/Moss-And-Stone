#pragma once
#include "Scene.h"
#include "Fields.h"
#include "Prefab.h"
#include "AnimationStateNodes.h"

namespace FE2D {
    class FOR_API SceneSerializer {
    public:
        SceneSerializer(Scene* scene) : m_Scene(scene) {}

        bool Serialize(const std::filesystem::path& full_path);
        bool Deserialize(const std::filesystem::path& full_path);

    public:
        template<typename T> requires std::is_same_v<T, PrefabEntity> || std::is_same_v<T, Entity>
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
                SceneSerializer::save_value(component.layer, j_component, "layer");

                j["PhysicsComponent"] = j_component;
            }

            if (entity.HasComponent<AnimatorComponent>()) {
                auto& component = entity.GetComponent<AnimatorComponent>();

                json j_component;
				SceneSerializer::save_value(component.current_state, j_component, "current_state");
                SceneSerializer::save_value(component.current_time, j_component, "current_time");
                SceneSerializer::save_array(component.states, j_component, "states", [](const std::unique_ptr<IStateNode>& e) -> json {
                    json j;
                    if (e) {
                        if (auto state = dynamic_cast<AnimationStateNode*>(e.get())) {
                            j["type"] = "AnimationState";
                            SceneSerializer::save_resource_id(state->animation_id, j, "animation_id");
                            SceneSerializer::save_value(state->name, j, "name");
                            SceneSerializer::save_value(state->looping, j, "looping");
                        }
                        else if (auto blend = dynamic_cast<BlendTreeNode*>(e.get())) {
                            j["type"] = "BlendTree";
                            SceneSerializer::save_value(blend->name, j, "name");
                            SceneSerializer::save_value(blend->looping, j, "looping");
                            SceneSerializer::save_value(blend->parameter_name_x, j, "parameter_name_x");
                            SceneSerializer::save_value(blend->parameter_name_y, j, "parameter_name_y");

                            SceneSerializer::save_array(blend->animation_points, j, "animation_points", [](const BlendTreeNode::AnimationPoint& e) -> json {
                                json j;
                                SceneSerializer::save_vec2(e.first, j, "direction");
                                SceneSerializer::save_resource_id(e.second, j, "animation");
                                return j;
                                });
                        }
                    }
                    return j;
                    });
                SceneSerializer::save_array(component.transitions, j_component, "transitions", [](const AnimationTransition& e) -> json {
                    json j;
                    SceneSerializer::save_value(e.from_state, j, "from_state");
                    SceneSerializer::save_value(e.to_state, j, "to_state");
                    SceneSerializer::save_array(e.conditions, j, "conditions", [](const AnimationCondition& e2)->json {
                        json j;
                        SceneSerializer::save_value(e2.condition_type, j, "condition");
                        SceneSerializer::save_value(e2.parameter_name, j, "parameter_name");
                        SceneSerializer::save_value(e2.value, j, "value");
                        return j;
                        });
                    return j;
                    });
                SceneSerializer::save_array(component.parameters, j_component, "parameters", [](const std::pair<std::string, AnimationParameter>& e)->json {
                    json j;
                    SceneSerializer::save_value(e.first, j, "first");
                    SceneSerializer::save_value(e.second.value, j, "second.value");
                    return j;
                    });

                j["AnimatorComponent"] = j_component;
            }

            if (entity.HasComponent<NativeScriptComponent>()) {
                auto& component = entity.GetComponent<NativeScriptComponent>();

                json j_component;
                SceneSerializer::save_value(component.script_name, j_component, "script_name");
                j_component["script_data"] = component.instance ? component.instance->Serialize() : json(); // this must work like this

                j["NativeScriptComponent"] = j_component;
            }

            return j;
        }

        inline static std::vector<ComponentsVariant> DeserializeComponents(const json& j) {
            std::vector<ComponentsVariant> deserialized_components;

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
                SceneSerializer::load_value(component.layer, j_component, "layer");

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("AnimatorComponent")) {
                AnimatorComponent component;
                const json& j_component = j["AnimatorComponent"];

                SceneSerializer::load_value(component.current_state, j_component, "current_state");
                SceneSerializer::load_value(component.current_time, j_component, "current_time");

                component.states.clear();
                if (j_component.contains("states")) {
                    for (const auto& j_state : j_component["states"]) {
                        if (!j_state.contains("type")) continue;

                        std::string type = j_state["type"];

                        if (type == "AnimationState") {
                            auto node = std::make_unique<AnimationStateNode>();

                            SceneSerializer::load_resource_id(node->animation_id, j_state, "animation_id");
                            SceneSerializer::load_value(node->name, j_state, "name");
                            SceneSerializer::load_value(node->looping, j_state, "looping");

                            component.states.emplace_back(std::move(node));
                        }
                        else if (type == "BlendTree") {
                            auto node = std::make_unique<BlendTreeNode>();

                            SceneSerializer::load_value(node->name, j_state, "name");
                            SceneSerializer::load_value(node->looping, j_state, "looping");
                            SceneSerializer::load_value(node->parameter_name_x, j_state, "parameter_name_x");
                            SceneSerializer::load_value(node->parameter_name_y, j_state, "parameter_name_y");
                            
                            SceneSerializer::load_array(node->animation_points, j_state, "animation_points", [](const json& j) -> std::pair<vec2, ResourceID<Animation>> {
                                std::pair<vec2, ResourceID<Animation>> pair;
                                SceneSerializer::load_vec2(pair.first, j, "direction");
                                SceneSerializer::load_resource_id(pair.second, j, "animation");
                                return pair;
                                });

                            component.states.emplace_back(std::move(node));
                        }
                    }
                }

                if (j_component.contains("transitions")) {
                    for (const auto& j_trans : j_component["transitions"]) {
                        AnimationTransition trans;
                        SceneSerializer::load_value(trans.from_state, j_trans, "from_state");
                        SceneSerializer::load_value(trans.to_state, j_trans, "to_state");

                        if (j_trans.contains("conditions")) {
                            for (const auto& j_cond : j_trans["conditions"]) {
                                AnimationCondition cond;
                                SceneSerializer::load_value(cond.parameter_name, j_cond, "parameter_name");
                                SceneSerializer::load_value(cond.condition_type, j_cond, "condition");
                                SceneSerializer::load_value(cond.value, j_cond, "value");
                                trans.conditions.push_back(cond);
                            }
                        }

                        component.transitions.push_back(std::move(trans));
                    }
                }

                if (j_component.contains("parameters")) {
                    for (const auto& j_param : j_component["parameters"]) {
                        std::string name;
                        std::string param_name;

                        SceneSerializer::load_value(name, j_param, "first");
                        SceneSerializer::load_value(param_name, j_param, "second.name");

                        AnimationParameter param;

                        if      (j_param["second.value"].is_boolean       ()) param.value =         j_param["second.value"]           .get<bool >();
                        else if (j_param["second.value"].is_number_integer()) param.value =         j_param["second.value"]           .get<int  >();
                        else if (j_param["second.value"].is_number_float  ()) param.value =         j_param["second.value"]           .get<float>();
                        else if (j_param["second.value"].is_object        ()) param.value = Trigger(j_param["second.value"]["trigger"].get<bool >());

                        component.parameters.emplace(name, param);
                    }
                }

                deserialized_components.emplace_back(std::move(component));
            }

            if (j.contains("NativeScriptComponent")) {
                NativeScriptComponent component;

                const json& j_component = j["NativeScriptComponent"];
                SceneSerializer::load_value(component.script_name, j_component, "script_name");
                component.instance = ScriptFactory::Instance().CreateScript(component.script_name);
                if (component.instance)
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
        inline static void save_field(const IField* field, json& j, const std::string& name) {
            if (!field || field->uuid == FE2D::UUID()) return;
            j[name] = field->uuid.ToString();
        }

        inline static void save_uuid(FE2D::UUID uuid, json& j, const std::string& name) {
            if (uuid.ToString().empty()) return;
            j[name] = uuid.ToString();
        }

        template<typename T> requires std::is_base_of_v<IResource, T>
        inline static void save_resource_id(ResourceID<T> id, json& j, const std::string& name) {
            j[name] = id.uuid.ToString();
        }

        template<typename T> requires (!std::is_base_of_v<IResource, T> && (std::is_trivial_v<T> || (std::is_same_v<T, std::string>)))
        inline static void save_value(const T& value, json& j, const std::string& name) {
            if constexpr (std::is_same_v<T, std::string>)
                if (value.empty()) return;
            j[name] = value;
        }

        template<typename... Ts>
        inline static void save_value(const std::variant<Ts...>& value, json& j, const std::string& name) {
            std::visit([&](const auto& v) {
                if      constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool   >) j[name]            = v;
                else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, float  >) j[name]            = v;
                else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int    >) j[name]            = v;
                else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, Trigger>) j[name]["trigger"] = v.is_triggered();
                else throw std::runtime_error("Unsupported type in std::variant for save_value");
                }, value);
        }

        template<typename Array, typename Func>
            requires (
                std::invocable<Func, typename Array::value_type>&&
                std::convertible_to<std::invoke_result_t<Func, typename Array::value_type>, json>
            )
            inline static void save_array(const Array& array, json& j, const std::string& name, Func&& func) {
            json j_array = json::array();
            for (const auto& e : array) {
                j_array.emplace_back(func(e));
            }
            j[name] = j_array;
        }

        inline static void save_value(const std::wstring& value, json& j, const std::string& name) {
            std::string load_value = FE2D::wstring_to_string(value);
            j[name] = load_value;
        }

        inline static void save_vec2(const vec2& value, json& j, const std::string& name) {
            j[name] = { value.x, value.y };
        }
        inline static void save_vec3(const vec3& value, json& j, const std::string& name) {
            j[name] = { value.x, value.y, value.z };
        }
        inline static void save_vec4(const vec4& value, json& j, const std::string& name) {
            j[name] = { value.x, value.y, value.z, value.w };
        }
    public:
        inline static void load_field(IField* field, const json& j, const std::string& name) {
            if (!j.contains(name)) return;
            field->uuid = FE2D::UUID(j[name].get<std::string>());
        }

        inline static void load_uuid(FE2D::UUID& uuid, const json& j, const std::string& name) {
            if (!j.contains(name)) return;
            uuid = FE2D::UUID(j[name].get<std::string>());
        }

        template<typename T> requires std::is_base_of_v<IResource, T>
        inline static void load_resource_id(ResourceID<T>& id, const json& j, const std::string& name) {
            if (!j.contains(name)) return;
            id.uuid = FE2D::UUID(j[name].get<std::string>());
        }

        template<typename T> requires (!std::is_base_of_v<IResource, T> && (std::is_trivial_v<T> || (std::is_same_v<T, std::string>)))
        inline static void load_value(T& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;
            value = j[name].get<T>();
        }

        template<typename... Ts>
        inline static void load_value(std::variant<Ts...>& value, const json& j, const std::string& name) {
            if (!j[name].contains(name)) return;
            
            if      (j[name].is_boolean())        value = j[name].get<bool>();
            else if (j[name].is_number_float())   value = j[name].get<float>();
            else if (j[name].is_number_integer()) value = j[name].get<int>();
            else if (j[name].is_object        ()) value = j[name]["trigger"].get<bool>();
        }

        template<typename Array, typename Func>
            requires (
                std::invocable<Func, const json&>&&
                std::convertible_to<std::invoke_result_t<Func, const json&>, typename Array::value_type>
            )
            inline static void load_array(Array& array, const json& j, const std::string& name, Func&& func) {
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

        inline static void load_value(std::wstring& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            std::string load_value = j[name].get<std::string>();
            value = FE2D::string_to_wstring(load_value);
        }

        inline static void load_vec2(vec2& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>() };
        }
        inline static void load_vec3(vec3& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>() };
        }
        inline static void load_vec4(vec4& value, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>(), load_value[3].get<float>() };
        }
	};
}
