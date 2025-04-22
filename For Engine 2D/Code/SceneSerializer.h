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
        template<typename T, std::enable_if<!std::is_same<T, std::wstring>::value, int>::type = 0>
        static void save_value(const T& value, json& j, const std::string& name) {
            j[name] = value;
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
        template<typename T, std::enable_if<!std::is_same<T, std::wstring>::value, int>::type = 0>
        static void load_value(T& value, const json& j, const std::string& name) {
            if (!j.contains(name))
                return;

            value = j[name].get<T>();
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
