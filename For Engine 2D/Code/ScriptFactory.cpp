#include "forpch.h"
#include "ScriptFactory.h"

ScriptableEntity* FE2D::ScriptFactory::CreateRenderer(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_FactoryMap.find(name);
    if (it == m_FactoryMap.end())
        return nullptr;

    return it->second();
}