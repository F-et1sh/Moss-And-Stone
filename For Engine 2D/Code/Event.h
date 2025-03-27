#pragma once

namespace FE2D {
    struct FOR_API Event {
    public:
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
    public:
        Event() = default;
        virtual ~Event() = default;
    public:
        EventType type = EventType::None;
    };
}