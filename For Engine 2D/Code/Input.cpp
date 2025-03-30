#include "forpch.h"
#include "Input.h"

void FE2D::Input::Release()
{
}

void FE2D::Input::Initialize(Window* window) {
	if (!window)
		FOR_RUNTIME_ERROR("Failed to Initialize Input. Window was nullptr");
	m_Window = window;

    window->SubscribeOnEvent(Event::EventType::KeyPressed, [&](const Event& e) {
        const KeyPressed* _event = static_cast<const KeyPressed*>(&e);
        m_KeyStates[_event->key] = true;
        });

    window->SubscribeOnEvent(Event::EventType::KeyReleased, [&](const Event& e) {
        const KeyReleased* _event = static_cast<const KeyReleased*>(&e);
        m_KeyStates[_event->key] = false;
        });
}

void FE2D::Input::UpdateDirection() {
    m_Direction = vec2();

    if (m_KeyStates[GLFW_KEY_W]) m_Direction.y =  1;
    if (m_KeyStates[GLFW_KEY_S]) m_Direction.y = -1;
    if (m_KeyStates[GLFW_KEY_D]) m_Direction.x =  1;
    if (m_KeyStates[GLFW_KEY_A]) m_Direction.x = -1;
}
