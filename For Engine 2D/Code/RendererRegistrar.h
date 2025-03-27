#pragma once
#include "RendererFactory.h"

namespace FE2D {
    template <typename RendererType, typename ComponentType>
    struct FOR_API RendererRegistrar {
        RendererRegistrar(const std::string_view& regular_name) {
            RendererFactory::Instance().RegisterRenderer<RendererType, ComponentType>(regular_name);
        }
    };
}