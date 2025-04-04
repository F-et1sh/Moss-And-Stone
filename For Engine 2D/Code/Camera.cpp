#include "forpch.h"
#include "Camera.h"

void FE2D::Camera::BindToWindow(Window* window) {
	if (!window)
		return;

	this->reset_pointers(); // reset if this camera is already bound

	window->SubscribeOnEvent(Event::EventType::WindowResized, [this](const Event& e) {
		const WindowResized* event = static_cast<const WindowResized*>(&e);
		if (event)
			m_VisionSize = event->size;
		});

	m_VisionSize = window->getResolution(); // set the vision size primarily
}
