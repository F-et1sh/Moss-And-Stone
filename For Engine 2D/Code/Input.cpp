#include "forpch.h"
#include "Input.h"

void FE2D::Input::Release()
{
}

void FE2D::Input::Initialize(Window* window) {
	if (!window)
		FOR_RUNTIME_ERROR("Failed to Initialize Input. Window was nullptr");
	m_Window = window;

	window->SubscribeOnEvent(Event::EventType::KeyPressed, [this](const Event& e) {
		const KeyPressed* event = static_cast<const KeyPressed*>(&e);
		this->k = event->key;
		});
	window->SubscribeOnEvent(Event::EventType::KeyReleased, [this](const Event& e) {
		const KeyPressed* event = static_cast<const KeyPressed*>(&e);
		this->k = 0;
		});
}
