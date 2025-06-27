#pragma once
#include "UUID.h"

namespace FE2D {
    class IMGUI; // forward declaration,

    class FOR_API IResource {
    public:
        IResource() = default;
        virtual ~IResource() = default;

        // legacy function. use destructor instead
        // if you overrided this - do NOT put it in the destructor
        // to free data use only Release() or only destructor, but never use them together
        virtual void Release() {}

        virtual bool LoadFromFile(const std::filesystem::path& file_path) = 0;
        virtual void UplopadToFile(const std::filesystem::path& file_path)const {}

        virtual void OnEditorDraw(IMGUI& imgui) {}

        virtual json Serialize()const { return json(); }
        virtual void Deserialize(const json& j) {}

        inline void setUUID(FE2D::UUID uuid)noexcept { m_UUID = uuid; }
        inline FE2D::UUID getUUID()const noexcept { return m_UUID; }

    private:
        FE2D::UUID m_UUID = FE2D::UUID(0);
    };
}