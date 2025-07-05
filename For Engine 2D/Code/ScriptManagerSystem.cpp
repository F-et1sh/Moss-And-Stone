#include "forpch.h"
#include "ScriptManagerSystem.h"

void FE2D::ScriptManagerSystem::OnEnd() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto view = registry.view<NativeScriptComponent>();
	for (auto e : view) {
		Entity entity = { e, m_Scene };
		auto& nsc = entity.GetComponent<NativeScriptComponent>();
		if (nsc.instance) {
			nsc.instance->OnEnd();
		}
	}
}

void FE2D::ScriptManagerSystem::OnStart() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto view = registry.view<NativeScriptComponent>();
	for (auto e : view) {
		Entity entity = { e, m_Scene };
		auto& nsc =  entity.GetComponent<NativeScriptComponent>();
		if (nsc.instance) {
			nsc.instance->OnStart();
			nsc.instance->OnAwake();
		}
	}
}

void FE2D::ScriptManagerSystem::OnUpdate() {
	entt::registry& registry = this->m_Scene->getRegistry();
	double delta_time = this->m_Window->getDeltaTime();

	std::vector<Entity> entities_to_destroy;

	auto view = registry.view<NativeScriptComponent>();
	for (auto e : view) {
		Entity entity = { e, m_Scene };
		auto& nsc = entity.GetComponent<NativeScriptComponent>();
		if (nsc.instance) {
			nsc.instance->OnUpdate(delta_time);

			if (nsc.instance->IsDestroyed()) {
				entities_to_destroy.emplace_back(entity);
			}
		}
	}

	for (auto e : entities_to_destroy) {
		m_Scene->DestroyEntity(e);
	}
}
