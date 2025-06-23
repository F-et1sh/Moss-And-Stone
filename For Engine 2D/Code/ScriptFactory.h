#pragma once
#include "Window.h"

namespace FE2D {
    class ScriptableEntity; // forward declaration

    class FOR_API ScriptFactory {
    private:
        using ScriptEntry = std::function<std::unique_ptr<ScriptableEntity>()>;
    public:
        FOR_CLASS_NONCOPYABLE(ScriptFactory)

        static ScriptFactory& Instance() {
            static ScriptFactory script_factory;
            return script_factory;
        }

        template<typename T> requires std::is_base_of_v<ScriptableEntity, T>
        inline bool RegisterScript(const std::string& name) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            if (m_FactoryMap.find(name) == m_FactoryMap.end()) {
                m_FactoryMap.emplace(name, []()->std::unique_ptr<ScriptableEntity> { return std::make_unique<T>(); });
                return true;
            }

            SAY("ERROR : Script already registered\nType : " << typeid(T).name() << "\nName : " << name.c_str());
            return false;
        }

        inline const auto& GetRegisteredScripts()const noexcept { return m_FactoryMap; }
        std::unique_ptr<ScriptableEntity> CreateScript(const std::string& name, Entity entity);

    private:
        ScriptFactory() = default;
        ~ScriptFactory() = default;

    private:
        std::unordered_map<std::string, ScriptEntry> m_FactoryMap;
        mutable std::mutex m_Mutex;
    };

    template <typename T>
    struct FOR_API ScriptRegistrar {
        ScriptRegistrar(const std::string& name) {
            ScriptFactory::Instance().RegisterScript<T>(name);
        }
    };

#define FOR_SCRIPT_REGISTER(script_type, name) inline static FE2D::ScriptRegistrar<script_type> script_type##_script_register(name);
}