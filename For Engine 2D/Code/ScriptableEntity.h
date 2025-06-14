#pragma once
#include "ScriptFactory.h"

namespace FE2D {
	class FOR_API ScriptableEntity : public Entity {
	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;

		template<typename T>
		inline T& GetComponent() { return this->GetComponent<T>(); }

		virtual void OnEnd() {}
		virtual void OnStart() {}
		virtual void OnUpdate(double deltaTime) {}

		virtual void OnEditorPanel(IMGUI& imgui) {}
	};
}