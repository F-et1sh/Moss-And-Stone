#pragma once
#include "ComponentFactory.h"

namespace FE2D {
    template <typename T>
    struct FOR_API ComponentRegistrar {
        ComponentRegistrar(const std::string_view& regular_name) { 
            ComponentFactory::Instance().RegisterComponent<T>(regular_name);
        }
    };
}