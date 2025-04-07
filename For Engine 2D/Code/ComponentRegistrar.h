#pragma once
#include "ComponentFactory.h"

namespace FE2D {
    template <typename T>
    struct FOR_API ComponentRegistrar {
        ComponentRegistrar(const std::string_view& pretty_name) { 
            ComponentFactory::Instance().RegisterComponent<T>(pretty_name); 
        }
        ~ComponentRegistrar() = default;
    };
#define FOR_COMPONENT_REGISTER(T, pretty_name) static FE2D::ComponentRegistrar<T> T##_register(pretty_name);
}