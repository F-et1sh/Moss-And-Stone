#pragma once
#include "Scene.h"
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

			for (auto field : m_Fields) field->set_context(m_Scene);

			this->OnAwake();
		}

		inline Entity this_entity()const { 
			if (!m_Scene) FOR_RUNTIME_ERROR("Failed to get this entity in a scrpt\nScene was nullptr");
			return { m_EntityHandle, m_Scene };
		}
		inline void destroy_this()const {
			m_Scene->DestroyEntity(this_entity());
		}

		inline Scene& get_scene()const {
			if (!m_Scene) FOR_RUNTIME_ERROR("Failed to get scene in a scrpt\nScene was nullptr");
			return *m_Scene;
		}
		inline ResourceManager& get_resource_manager()const {
			if (!m_Scene) FOR_RUNTIME_ERROR("Failed to get resource manager in a scrpt\nScene was nullptr");
			return m_Scene->getResourceManager();
		}

		template<typename T>
		inline ComponentField<T>& create_component_field() {
			auto field = new ComponentField<T>();
			m_Fields.emplace_back(field);
			return *field;
		}

		inline EntityField& create_entity_field() {
			auto field = new EntityField();
			m_Fields.emplace_back(field);
			return *field;
		}

		void subscribe_to_event(EventType type, std::function<void(const IEvent& e)>&& func);
		bool is_key_pressed(int key);
		bool is_mouse_button_pressed(int button);
			
		void OnEnd() { m_EventSubscription.clear(); }

		virtual void OnStart() {}
		virtual void OnAwake() {}
		virtual void OnUpdate(double deltaTime) {}

		virtual json Serialize()const { return json(); }
		virtual void Deserialize(const json& j) {}

		virtual void OnEditorPanel(IMGUI& imgui) {}

	private:
		std::vector<EventSubscription> m_EventSubscription;
		std::vector<IField*> m_Fields;

	public:
		virtual std::unique_ptr<ScriptableEntity> clone()const = 0;
	};

#define GENERATED_BODY(T) \
public: \
    std::unique_ptr<ScriptableEntity> clone()const override { \
        return std::make_unique<T>(*this); \
    }

#define FOR_RESOURCE_FIELD(type, name)  ResourceID<type> name;

#define FOR_COMPONENT_FIELD(type, name)	ComponentField<type>& name = create_component_field<type>()
#define FOR_ENTITY_FIELD(name)			EntityField& name = create_entity_field()

#define FOR_SAVE_FIELD(field)			SceneSerializer::save_field(static_cast<const IField*>(&field), j, #field)
#define FOR_LOAD_FIELD(field)			SceneSerializer::load_field(static_cast<IField*>(&field), j, #field)

#define FOR_SAVE_RESOURCE(resource)		SceneSerializer::save_resource_id(resource, j, #resource)
#define FOR_LOAD_RESOURCE(resource)		SceneSerializer::load_resource_id(resource, j, #resource)

#define FOR_SAVE_VALUE(value)			SceneSerializer::save_value(value, j, #value)
#define FOR_LOAD_VALUE(value)			SceneSerializer::load_value(value, j, #value)
										
#define FOR_SAVE_VECTOR_2(value)		SceneSerializer::save_vec2(value, j, #value)
#define FOR_LOAD_VECTOR_2(value)		SceneSerializer::load_vec2(value, j, #value)
										
#define FOR_SAVE_VECTOR_3(value)		SceneSerializer::save_vec3(value, j, #value)
#define FOR_LOAD_VECTOR_3(value)		SceneSerializer::load_vec3(value, j, #value)
										
#define FOR_SAVE_VECTOR_4(value)		SceneSerializer::save_vec4(value, j, #value)
#define FOR_LOAD_VECTOR_4(value)		SceneSerializer::load_vec4(value, j, #value)
}