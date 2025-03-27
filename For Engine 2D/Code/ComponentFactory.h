#pragma once
#include "Component.h"

namespace FE2D {
    class FOR_API ComponentFactory {
    public:
        ComponentFactory() = default;
        ~ComponentFactory() = default;
    public:
        static ComponentFactory& Instance();

        template<typename T>
        bool RegisterComponent(const std::string_view& name) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            static_assert(std::is_base_of<UComponent, T>::value, "T must be derived from UComponent");

            const std::string type_name = typeid(T).name();

            if (m_FactoryMap.find(type_name) == m_FactoryMap.end()) {
                m_FactoryMap[type_name].first = name;
                m_FactoryMap[type_name].second = []()->UComponent* { return new T(); };
                return true;
            }

            SAY("ERROR : Component already Registered\nComponent type : " << type_name.c_str() << "\nComponent name : " << name.data());
            return false;
        }

        // std::string - typeid(T).name()
        // std::string_view - regular name of the component. Example : class FE2D::UTransformComponent --> Transform
        // std::function<UComponent*()> - to create new components
        const std::unordered_map<std::string, std::pair<std::string_view, std::function<UComponent*()>>>& getRegisteredComponents()const noexcept {
            return m_FactoryMap;
        }

        UComponent* CreateComponent(const std::string& type_name);

        const std::string_view& getComponentName(const std::string& type_name);

    private:
        // std::string - typeid(T).name()
        // std::string_view - regular name of the component. Example : class FE2D::UTransformComponent --> Transform
        // std::function<UComponent*()> - to create new components
        std::unordered_map<std::string, std::pair<std::string_view, std::function<UComponent*()>>> m_FactoryMap;
    private:
        mutable std::mutex m_Mutex;
    };
}
