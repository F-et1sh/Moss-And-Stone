#pragma once
#include "ScriptFactory.h"

namespace FE2D {
	class FOR_API ScriptableEntity : public Entity {
	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;

		void setContext(Entity entity)noexcept {
			this->m_EntityHandle = entity.m_EntityHandle;
			this->m_Scene = entity.m_Scene;
		}

		virtual void OnStart() {}
		virtual void OnUpdate(double deltaTime) {}

		virtual json Serialize()const { return json(); }
		virtual void Deserialize(const json& j) {}

		virtual void OnEditorPanel(IMGUI& imgui) {}
	};
}