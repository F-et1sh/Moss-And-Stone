#include "forpch.h"
#include "ScriptableEntity.h"

void FE2D::ScriptableEntity::subscribe_to_event(EventType type, std::function<void(const IEvent& e)>&& func) {
	m_EventSubscription.push_back({});
	auto& subs = m_EventSubscription.back();
	m_Scene->getContextWindow().SubscribeToEvent(subs, type, func);
}

bool FE2D::ScriptableEntity::is_key_pressed(int key) {
	return glfwGetKey(m_Scene->getContextWindow().reference(), key);
}

bool FE2D::ScriptableEntity::is_mouse_button_pressed(int button) {
	return glfwGetMouseButton(m_Scene->getContextWindow().reference(), button);
}
