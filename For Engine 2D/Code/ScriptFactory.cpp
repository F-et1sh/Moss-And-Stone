#include "forpch.h"
#include "ScriptFactory.h"

std::unique_ptr<ScriptableEntity> FE2D::ScriptFactory::CreateScript(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_FactoryMap.find(name);
    if (it == m_FactoryMap.end())
        return nullptr;

    std::unique_ptr<ScriptableEntity> scriptable_entity = it->second();

    return scriptable_entity;
}
