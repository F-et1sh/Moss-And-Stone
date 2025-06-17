#pragma once
#include "EventDispatcher.h"

namespace FE2D {
	class Window; // forward declaration

	struct FOR_API EventSubscription {
	private:
		Window* window = nullptr;
		EventType event_type = EventType::None;
		EventDispatcher::ListenerID event_index = 0;
	
	public:
		// this function will be called automatically
		void unsubscribe();
		inline void set_context(Window* window, EventType event_type, EventDispatcher::ListenerID event_index)noexcept {
			this->window = window;
			this->event_type = event_type;
			this->event_index = event_index;
		}
		
		EventSubscription() = default;

		EventSubscription(Window* window, EventType event_type, EventDispatcher::ListenerID event_index) : window(window), event_type(event_type), event_index(event_index) {}
		~EventSubscription() { this->unsubscribe(); }
	};
}