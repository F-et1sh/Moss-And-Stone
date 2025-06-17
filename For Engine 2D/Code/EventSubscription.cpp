#include "forpch.h"
#include "EventSubscription.h"



void FE2D::EventSubscription::unsubscribe() {
	if (window) window->UnsubscribeFromEvent(event_type, event_index);
	window = nullptr;
}
