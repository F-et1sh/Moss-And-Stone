#pragma once

namespace FE2D {
	// Serializable - it's a class to inherit
	// Using it you can serialize/deserialize your class
	class FOR_API Serializable {
	public:
		Serializable() = default;
		~Serializable() = default;

        virtual json Serialize()const { return json(); }
        // After deserialization, the object must be fully working
        // i mean if the object has an Initialize() function
        // it must be initialized in this, Deserailize(), function
        virtual void Deserialize(const json& j) {}

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
            value = j.at(name).get<T>();
        }

        static void load_value(std::wstring& value, const json& j, const std::string& name) {
            std::string load_value = j.at(name).get<std::string>();
            value = FE2D::string_to_wstring(load_value);
        }

        static void load_vec2(vec2& value, const json& j, const std::string& name) {
            auto load_value = j.at(name);
            value = { load_value[0].get<float>(), load_value[1].get<float>() };
        }
        static void load_vec3(vec3& value, const json& j, const std::string& name) {
            auto load_value = j.at(name);
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>() };
        }
        static void load_vec4(vec4& value, const json& j, const std::string& name) {
            auto load_value = j.at(name);
            value = { load_value[0].get<float>(), load_value[1].get<float>(), load_value[2].get<float>(), load_value[3].get<float>() };
        }
	};
}