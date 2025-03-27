#include "forpch.h"
#include "ComponentFactory.h"

ComponentFactory& FE2D::ComponentFactory::Instance() {
    static ComponentFactory instance;
    return instance;
}

UComponent* FE2D::ComponentFactory::CreateComponent(const std::string& type_name) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_FactoryMap.find(type_name);
    if (it == m_FactoryMap.end())
        FOR_RUNTIME_ERROR("Failed to create a component\nIt's not registered\nType name : " + type_name);

    return it->second.second();
}

const std::string_view& FE2D::ComponentFactory::getComponentName(const std::string& type_name) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_FactoryMap.find(type_name);
    if (it == m_FactoryMap.end())
        SAY("ERROR : Component name not found\nIt's not registered\nType name : " << type_name.c_str());

    return it->second.first; // regualar name of the component
}