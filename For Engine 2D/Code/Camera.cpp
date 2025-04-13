#include "forpch.h"
#include "Camera.h"

void FE2D::Camera::BindToWindow(Window* window) {
	if (!window)
		return;

	this->reset_pointers(); // reset if this camera is already bound

	m_EventIndex = window->SubscribeOnEvent(Event::EventType::WindowResized, [&](const Event& e) {
		const WindowResized* event = static_cast<const WindowResized*>(&e);
		m_VisionSize = event->size;
		});

	m_VisionSize = window->getResolution(); // set the vision size primarily

	m_Window = window;
}

inline void FE2D::Camera::UnbindWindow() noexcept {
	if (!m_Window)
		return;

	m_Window->UnsubscribeOnEvent(Event::EventType::WindowResized, m_EventIndex);
	m_Window = nullptr;
}
