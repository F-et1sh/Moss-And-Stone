#pragma once

namespace FE2D {
    /* forward declarations */
    class Scene;
    class Entity;

	class FOR_API SceneSerializer {
	public:
		SceneSerializer(Scene* scene) : m_Scene(scene) {}

		bool Serialize(const std::filesystem::path& full_path);
        bool Deserialize(const std::filesystem::path& full_path);

    private:
        void SerializeEntity(json& j, Entity entity);
        
        void SerializeSceneInfo(json& j);
        bool DeserializeSceneInfo(const json& j);

	public:
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
        template<typename T> requires std::is_base_of_v<IResource, T>
        static void load_resource_id(ResourceID<T>& id, const json& j, const std::string& name) {
            if (!j.contains(name)) return;

            id.uuid = FE2D::UUID(j[name].get<std::string>());
        }

        template<typename T, typename = std::enable_if_t<!std::is_same_v<T, std::wstring> && !std::_Is_specialization_v<std::remove_cvref_t<T>, std::vector> && !std::is_pointer_v<T>>> requires (!std::is_base_of_v<IResource, T>)
        static void load_value(T& value, const json& j, const std::string& name) {
            if (!j.contains(name))
                return;

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
            if (!j.contains(name))
                return;

            std::string load_value = j[name].get<std::string>();
            value = FE2D::string_to_wstring(load_value);
        }

        static void load_vec2(vec2& value, const json& j, const std::string& name) {
            if (!j.contains(name))
                return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>() };
        }
        static void load_vec3(vec3& value, const json& j, const std::string& name) {
            if (!j.contains(name))
                return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>() };
        }
        static void load_vec4(vec4& value, const json& j, const std::string& name) {
            if (!j.contains(name))
                return;

            auto& load_value = j[name];
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>(), load_value[3].get<float>() };
        }
	private:
		Scene* m_Scene = nullptr;
	};

}
