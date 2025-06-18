#pragma once
#include "IEvent.h"

namespace FE2D {
    class FOR_API EventDispatcher {
    public:
        EventDispatcher() = default;
        ~EventDispatcher() = default;
    
        using Listener = std::function<void(const IEvent&)>;
        using ListenerID = size_t;
    
        // This function will return you listener's index you added
        // With this index you can unsubscribe on an event
        inline ListenerID subscribe(const EventType& type, const Listener& listener) {
            m_Listeners[type].emplace(m_NextIndex, listener);
            return m_NextIndex++; // Increase then return the Next Index
        }

        inline bool unsubscribe(const EventType& type, ListenerID id) {
            auto it = m_Listeners.find(type);
            if (it == m_Listeners.end()) return false;
            it->second.erase(id);
            return true;
        }

        inline void dispatch(const IEvent& event)noexcept {
            // do not check for errors here. operator[] will add unexist event type automaticly
            auto& typeListeners = m_Listeners[event.type];
            // handle all of events here
            for (const auto& listener : typeListeners) listener.second(event);
        }

    private:
        ListenerID m_NextIndex = 0;
        std::unordered_map<EventType, std::unordered_map<ListenerID, Listener>> m_Listeners;
    };
}