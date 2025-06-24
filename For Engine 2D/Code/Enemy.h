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
		FOR_SAVE_COMPONENT_FIELD(transform);
		//FOR_SAVE_ENTITY(player);
		FOR_SAVE_VALUE(speed);
		return j;
	}
	void Deserialize(const json& j)override {
		FOR_LOAD_COMPONENT_FIELD(transform);
		//FOR_LOAD_ENTITY(player);
		FOR_LOAD_VALUE(speed);
	}

	void OnEditorPanel(IMGUI& imgui)override {
		imgui.EntityPayload("Player", player);
		imgui.DragFloat("Speed", speed);
	}
};