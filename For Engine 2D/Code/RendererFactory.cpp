#include "forpch.h"
#include "RendererFactory.h"

RendererFactory& FE2D::RendererFactory::Instance() {
    static RendererFactory instance;
    return instance;
}

Renderer* FE2D::RendererFactory::CreateRenderer(size_t hash_code) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_FactoryMap.find(hash_code);
    if (it == m_FactoryMap.end())
        return nullptr;
    
    return it->second.second();
}

const std::string_view& FE2D::RendererFactory::getRendererName(size_t hash_code) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_FactoryMap.find(hash_code);
    if (it == m_FactoryMap.end())
        SAY("ERROR : Renderer name not found\nIt's not registered\nType hash code : " << hash_code);

    return it->second.first; // regualar name of the Renderer
}