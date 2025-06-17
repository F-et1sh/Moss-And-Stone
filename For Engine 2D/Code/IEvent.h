#pragma once

namespace FE2D {
    enum class EventType {
        None = 0,

        WindowClosed,
        WindowResized,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,

        TextEntered,
        KeyPressed,
        KeyReleased,
        KeyHeld,

        MouseWheelScrolled,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseEntered,
        MouseLeft,

        DropPath
    };

    struct FOR_API IEvent {
        EventType type = EventType::None;
        
        IEvent() = default;
        virtual ~IEvent() = default;
    };
}