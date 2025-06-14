#pragma once

namespace FE2D {
	class FOR_API Script : public ScriptableEntity {
	public:
		Script() = default;
		~Script() = default;

		ComponentField<TransformComponent> transform_this;
		ComponentField<TransformComponent> transform;
		Entity entity;

		void OnEnd()override {}
		void OnStart()override {
			SAY("Script was initialized");
			transform_this = { *this };
		}
		void OnUpdate(double deltaTime)override {

		}

		void OnEditorPanel(IMGUI& imgui)override {
			imgui.ComponentPayload(transform);
			imgui.EntityPayload(entity);
		}
	};
}