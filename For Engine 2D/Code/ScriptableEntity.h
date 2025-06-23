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

	public:
		virtual std::unique_ptr<ScriptableEntity> clone()const = 0;
	};
#define GENERATED_BODY(T) \
public: \
    std::unique_ptr<ScriptableEntity> clone()const override { \
        return std::make_unique<T>(*this); \
    }
}