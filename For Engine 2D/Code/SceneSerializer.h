#pragma once

namespace FE2D {
    /* forward declarations */
    class FOR_API Scene;
    class FOR_API Entity;

	class FOR_API SceneSerializer {
	public:
		SceneSerializer(Scene* scene) : m_Scene(scene) {}

		bool Serialize(const std::filesystem::path& filepath);
        bool Deserialize(const std::filesystem::path& filepath);

    private:
        void SerializeEntity(json& j, Entity entity);
        void SerializeSceneInfo(json& j);

	public:
        template<typename T> requires std::is_base_of_v<IResource, T>
        static void save_resource_id(ResourceID<T> id, json& j, const std::string& name) {
            j[name] = id.uuid.ToString();
        }

        template<typename T, typename = std::enable_if_t<!std::is_same_v<T, std::wstring> && !std::_Is_specialization_v<std::remove_cvref_t<T>, std::vector> && !std::is_pointer_v<T>>> requires (!std::is_base_of_v<IResource, T>)
        static void save_value(const T& value, json& j, const std::string& name) {
            j[name] = value;
        }

        template <typename T>
        static void save_vector(const std::vector<T>& data, json& j, const std::string& name, const std::function<void(const std::remove_cvref_t<T>& e, json& j)>& func) {
            size_t size = data.size();
            j[name] = json::array();

            for (size_t i = 0; i < size; i++) {
                json e;

                func(data[i], e);

                j[name].emplace_back(e);
            }
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
            if (!j.contains(name))
                return;

            id.uuid = FE2D::UUID::FromString(j[name].get<std::string>());
        }

        template<typename T, typename = std::enable_if_t<!std::is_same_v<T, std::wstring> && !std::_Is_specialization_v<std::remove_cvref_t<T>, std::vector> && !std::is_pointer_v<T>>> requires (!std::is_base_of_v<IResource, T>)
        static void load_value(T& value, const json& j, const std::string& name) {
            if (!j.contains(name))
                return;

            value = j[name].get<T>();
        }

        template <typename T>
        static void load_vector(std::vector<T>& data, json& j, const std::string& name, const std::function<void(std::remove_cvref_t<T>& e, const json& j)>& func) {
            if (!j.contains(name))
                return;

            if (!j[name].is_array())
                return;

            json array = j[name];
            size_t size = array.size();
            
            data.reserve(size);

            for (size_t i = 0; i < size; i++) {
                T e;
                
                func(e, array[i]);

                data.emplace_back(e);
            }
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
