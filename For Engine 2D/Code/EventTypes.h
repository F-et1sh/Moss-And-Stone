#pragma once
#include "IEvent.h"

namespace FE2D {

#pragma region Window

    struct FOR_API WindowClosed : public IEvent
    {
        WindowClosed() { type = EventType::WindowClosed; }
    };
    struct FOR_API WindowResized : public IEvent
    {
        vec2 size = vec2();
        WindowResized(vec2 size) : size(size) { type = EventType::WindowResized; }
    };
    struct FOR_API WindowFocus : public IEvent
    {
        WindowFocus() { type = EventType::WindowFocus; }
    };
    struct FOR_API WindowLostFocus : public IEvent
    {
        WindowLostFocus() { type = EventType::WindowLostFocus; }
    };
    struct FOR_API WindowMoved : public IEvent
    {
        vec2 position = vec2();
        WindowMoved(vec2 position) : position(position) { type = EventType::WindowMoved; }
    };

#pragma endregion

#pragma region Keyboard

    struct FOR_API TextEntered : public IEvent
    {
        unsigned int unicode = 0;
        TextEntered(unsigned int unicode) : unicode(unicode) { type = EventType::TextEntered; }
    };

    struct FOR_API KeyPressed : public IEvent
    {
        int key = 0;
        int scancode = 0;

        bool alt = false;
        bool ctrl = false;
        bool shift = false;
        bool super = false;

        KeyPressed(int key, int scancode, bool alt, bool ctrl, bool shift, bool super) :
            key(key), scancode(scancode), alt(alt), ctrl(ctrl), shift(shift), super(super) {
            type = EventType::KeyPressed;
        }
    };

    struct FOR_API KeyReleased : public IEvent
    {
        int key = 0;
        int scancode = 0;

        bool alt = false;
        bool ctrl = false;
        bool shift = false;
        bool super = false;

        KeyReleased(int key, int scancode, bool alt, bool ctrl, bool shift, bool super) :
            key(key), scancode(scancode), alt(alt), ctrl(ctrl), shift(shift), super(super) {
            type = EventType::KeyReleased;
        }
    };

    struct FOR_API KeyHeld : public IEvent
    {
        int key = 0;
        int scancode = 0;

        bool alt = false;
        bool ctrl = false;
        bool shift = false;
        bool super = false;

        KeyHeld(int key, int scancode, bool alt, bool ctrl, bool shift, bool super) :
            key(key), scancode(scancode), alt(alt), ctrl(ctrl), shift(shift), super(super) {
            type = EventType::KeyHeld;
        }
    };

#pragma endregion

#pragma region Mouse

    struct FOR_API MouseWheelScrolled : public IEvent
    {
        vec2 offset = vec2();
        MouseWheelScrolled(vec2 offset) : offset(offset) { type = EventType::MouseWheelScrolled; }
    };

    struct FOR_API MouseButtonPressed : public IEvent
    {
        int button = 0;
        MouseButtonPressed(int button) : button(button) { type = EventType::MouseButtonPressed; }
    };

    struct FOR_API MouseButtonReleased : public IEvent
    {
        int button = 0;
        MouseButtonReleased(int button) : button(button) { type = EventType::MouseButtonReleased; }
    };

    struct FOR_API MouseMoved : public IEvent
    {
        vec2 position = vec2();
        MouseMoved(vec2 position) : position(position) { type = EventType::MouseMoved; }
    };

    struct FOR_API MouseEntered : public IEvent
    {
        MouseEntered() { type = EventType::MouseEntered; }
    };

    struct FOR_API MouseLeft : public IEvent
    {
        MouseLeft() { type = EventType::MouseLeft; }
    };

#pragma endregion

#pragma region Drop Path

    struct FOR_API DropPath : public IEvent
    {
        std::vector<std::wstring> paths;
        DropPath(std::vector<std::wstring> paths) : paths(paths) { type = EventType::DropPath; }
    };

#pragma endregion

}