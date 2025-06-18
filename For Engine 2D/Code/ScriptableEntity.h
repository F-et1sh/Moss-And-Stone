#pragma once
#include "ScriptFactory.h"

namespace FE2D {
	class FOR_API ScriptableEntity : public Entity {
	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;

		inline void setContext(Entity entity)noexcept {
			m_EntityHandle = entity.m_EntityHandle;
			m_Scene = entity.m_Scene;
		}

		inline Entity this_entity()const noexcept { return { m_EntityHandle, m_Scene }; }

		void subscribe_to_event(EventType type, std::function<void(const IEvent& e)>&& func);
		bool is_key_pressed(int key);

		void OnEnd() { m_EventSubscription.clear(); }

		virtual void OnStart() {}
		virtual void OnUpdate(double deltaTime) {}

		virtual json Serialize()const { return json(); }
		virtual void Deserialize(const json& j) {}

		virtual void OnEditorPanel(IMGUI& imgui) {}
	
	private:
		std::vector<EventSubscription> m_EventSubscription;
	};
#define SAVE_COMPONENT_FIELD(field) SceneSerializer::save_component_field(field, j, #field);
#define LOAD_COMPONENT_FIELD(field) SceneSerializer::load_component_field(field, j, #field, m_Scene);
}