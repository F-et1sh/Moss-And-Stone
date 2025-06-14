#pragma once

namespace FE2D {
    class IMGUI; // forward declaration

    class FOR_API IResource {
    public:
        IResource() = default;
        virtual ~IResource() = default;

        virtual void Release() = 0;
        virtual bool LoadFromFile(const std::filesystem::path& file_path) = 0;
        virtual void UplopadToFile(const std::filesystem::path& file_path)const {}

        virtual void OnEditorDraw(IMGUI& imgui) {}

        virtual json Serialize()const { return json(); }
        virtual void Deserialize(const json& j) {}
    };
}