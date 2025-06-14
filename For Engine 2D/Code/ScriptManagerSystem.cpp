#include "forpch.h"
#include "ScriptManagerSystem.h"

void FE2D::ScriptManagerSystem::Initialize()
{
}

void FE2D::ScriptManagerSystem::OnStart() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto view = registry.view<NativeScriptComponent>();
	for (auto e : view) {
		Entity entity = { e, m_Scene };
		auto& nsc =  entity.GetComponent<NativeScriptComponent>();
		if (nsc.instance) {
			nsc.instance->OnStart();
		}
	}
}

void FE2D::ScriptManagerSystem::OnUpdate() {
	entt::registry& registry = this->m_Scene->getRegistry();
	double delta_time = this->m_Window->getDeltaTime();

	auto view = registry.view<NativeScriptComponent>();
	for (auto e : view) {
		Entity entity = { e, m_Scene };
		auto& nsc = entity.GetComponent<NativeScriptComponent>();
		if (nsc.instance) {
			nsc.instance->OnUpdate(delta_time);
		}
	}
}
