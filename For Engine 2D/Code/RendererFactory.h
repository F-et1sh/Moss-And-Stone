#pragma once
#include "Renderer.h"

namespace FE2D {
    class FOR_API RendererFactory {
    public:
        RendererFactory() = default;
        ~RendererFactory() = default;
    public:
        static RendererFactory& Instance();

        template<typename RendererType, typename ComponentType>
        bool RegisterRenderer(const std::string_view& name) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            static_assert(std::is_base_of<Renderer, RendererType>::value, "Renderer Type must be derived from Renderer");
            static_assert(std::is_base_of<UComponent, ComponentType>::value, "Component Type must be derived from UComponent");

            const size_t component_hash_code = typeid(ComponentType).hash_code();

            if (m_FactoryMap.find(component_hash_code) == m_FactoryMap.end()) {
                m_FactoryMap[component_hash_code].first = name;
                m_FactoryMap[component_hash_code].second = []()->Renderer* { return new RendererType(); };
                return true;
            }

            SAY("ERROR : Renderer already Registered" << 
                "\nRenderer type : " << typeid(RendererType).name() << 
                "\nComponent Type : " << typeid(ComponentType).name() << 
                "\nRenderer name : " << name.data());
            return false;
        }

        // size_t hash_code - UComponent hash_code - typeid(T).hash_code()
        // std::string_view - regular name of the Renderer. Example : class FE2D::SpriteRenderer --> SpriteRenderer
        // std::function<Renderer*()> - to create new Renderers
        const std::unordered_map<size_t, std::pair<std::string_view, std::function<Renderer* ()>>>& getRegisteredRenderers()const noexcept {
            return m_FactoryMap;
        }

        Renderer* CreateRenderer(size_t hash_code);

        const std::string_view& getRendererName(size_t hash_code);

    private:
        // size_t hash_code - UComponent hash_code - typeid(T).hash_code()
        // std::string_view - regular name of the Renderer. Example : class FE2D::SpriteRenderer --> SpriteRenderer
        // std::function<Renderer*()> - to create new Renderers
        std::unordered_map<size_t, std::pair<std::string_view, std::function<Renderer* ()>>> m_FactoryMap;
    private:
        mutable std::mutex m_Mutex;
    };
}
