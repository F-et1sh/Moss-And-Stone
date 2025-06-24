#pragma once
#include "ScriptFactory.h"
#include "Fields.h"

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

	protected:
		template<typename T>
		inline ComponentField<T> create_component_field()const noexcept {

		}

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

#define FOR_SAVE_COMPONENT_FIELD(field) SceneSerializer::save_component_field(field, j, #field)
#define FOR_LOAD_COMPONENT_FIELD(field) SceneSerializer::load_component_field(field, j, #field, m_Scene)

#define FOR_SAVE_ENTITY_FIELD(entity)	SceneSerializer::save_entity(entity, j, #entity)
#define FOR_LOAD_ENTITY_FIELD(entity)	SceneSerializer::load_entity(entity, j, #entity, m_Scene)

#define FOR_SAVE_VALUE(value)			SceneSerializer::save_value(value, j, #value)
#define FOR_LOAD_VALUE(value)			SceneSerializer::load_value(value, j, #value)

#define FOR_SAVE_VECTOR_2(value)		SceneSerializer::save_vec2(value, j, #value)
#define FOR_LOAD_VECTOR_2(value)		SceneSerializer::load_vec2(value, j, #value)

#define FOR_SAVE_VECTOR_3(value)		SceneSerializer::save_vec3(value, j, #value)
#define FOR_LOAD_VECTOR_3(value)		SceneSerializer::load_vec3(value, j, #value)

#define FOR_SAVE_VECTOR_4(value)		SceneSerializer::save_vec4(value, j, #value)
#define FOR_LOAD_VECTOR_4(value)		SceneSerializer::load_vec4(value, j, #value)
}