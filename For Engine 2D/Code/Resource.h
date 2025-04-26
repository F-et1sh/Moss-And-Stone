#pragma once

namespace FE2D {
    class FOR_API Resource {
    public:
        Resource() = default;
        ~Resource() = default;
    
        virtual void Release() = 0;
        virtual bool LoadFromFile(const std::wstring& file_path) = 0;

        virtual json Serialize()const = 0;
        virtual void Deserialize(const json& j) = 0;
    };
}