#pragma once

namespace FE2D {
	class FOR_API PlayerController : public ScriptableEntity {
	public:
		ComponentField<TransformComponent> transform;
		ComponentField<SpriteComponent> sprite;

		PlayerController() {

		}
		~PlayerController() {

		}

		void OnStart()override {
			sprite = this_entity();
			transform = this_entity();
		}

		json Serialize()const override {
			json j;
			return j;
		}

		void Deserialize(const json& j)override {

		}

		void OnEditorPanel(IMGUI& imgui)override {
			
		}
	};
}