#pragma once

class Enemy : public ScriptableEntity {
	
	GENERATED_BODY(Enemy)

public:
	Enemy() {}
	~Enemy() {}

	ComponentField<TransformComponent> transform;

	Entity player;
	float speed = 0.0f;

	void OnStart()override {
		transform = this_entity();
	}

	void OnUpdate(double deltaTime)override {
		auto& player_transform = player.GetComponent<TransformComponent>();

		vec2 direction = player_transform.position - transform->position;

		if (direction.x < 0.0f) transform->scale.x =  fabsf(transform->scale.x);
		if (direction.x > 0.0f) transform->scale.x = -fabsf(transform->scale.x);

		if (length(direction) != 0.0f) {
			direction = normalize(direction);
			transform->position += direction * static_cast<float>(deltaTime) * speed;
		}
	}

	json Serialize()const override{
		json j;
		SAVE_COMPONENT_FIELD(transform);
		SAVE_ENTITY(player);
		SAVE_VALUE(speed);
		return j;
	}
	void Deserialize(const json& j)override {
		LOAD_COMPONENT_FIELD(transform);
		LOAD_ENTITY(player);
		LOAD_VALUE(speed);
	}

	void OnEditorPanel(IMGUI& imgui)override {
		imgui.EntityPayload("Player", player);
		imgui.DragFloat("Speed", speed);
	}
};