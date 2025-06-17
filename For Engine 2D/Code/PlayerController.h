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

		json Serialize()const override{ 
			json j;
			SAVE_COMPONENT_FIELD(sprite)
			return j;
		}

		void Deserialize(const json& j)override {
			LOAD_COMPONENT_FIELD(sprite)
		}
	};
}