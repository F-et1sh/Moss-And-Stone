#pragma once

namespace FE2D {
    class FOR_API IMGUI;

    class FOR_API IResource {
    public:
        IResource() = default;
        ~IResource() = default;

        virtual void Release() = 0;
        virtual bool LoadFromFile(const std::filesystem::path& file_path) = 0;

        virtual void OnEditorDraw(IMGUI& imgui) {}

        virtual json Serialize()const = 0;
        virtual void Deserialize(const json& j) = 0;
    };
}