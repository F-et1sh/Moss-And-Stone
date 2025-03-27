#pragma once
#include "Serializable.h"
#include "ISelectable.h"

namespace FE2D {
    // Resource - it's a class to inherit
    // It's based on Serializable class
    // You have to use it with classes you loading from file, which has source, not only serialized metadata
    class FOR_API Resource : public Serializable, public ISelectable {
    public:
        Resource() = default;
        ~Resource() = default;
    
        virtual void Release() = 0;
        virtual bool LoadFromFile(const std::wstring& file_path) = 0;
    };
#define FOR_RESOURCE_REGISTER(type) static FE2D::ResourceRegistrar<type> type##_register;
}