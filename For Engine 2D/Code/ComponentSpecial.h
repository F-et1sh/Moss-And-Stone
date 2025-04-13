#pragma once

namespace FE2D {
    struct FOR_API ISerializableComponent {
        virtual json Serialize()const { return {}; }
        virtual void Deserialize(const json& j) {}

        virtual ~ISerializableComponent() = default;
    };

    struct FOR_API IDrawableUIComponent {
        virtual void DrawUI(IMGUI& imgui) {}

        virtual ~IDrawableUIComponent() = default;
    };
}