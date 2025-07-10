#pragma once

class Enemy : public ScriptableEntity {

	GENERATED_BODY(Enemy)

	void OnRegisterFields()override {
		FOR_REGISTER_FIELD(transform);
		FOR_REGISTER_FIELD(physics);
		FOR_REGISTER_FIELD(health);

		FOR_REGISTER_FIELD(player);
	}

public:

	Enemy()
	{
	}
	~Enemy()
	{
	}

	ComponentField<TransformComponent> transform;
	ComponentField<PhysicsComponent> physics;
	ComponentField<HealthComponent> health;

	EntityField player;

	float speed = 0.0f;

	void OnStart()override {
		transform = this_entity();
		physics = this_entity();
		health = this_entity();
	}

	void OnUpdate(double deltaTime)override {
		auto& player_transform = player->GetComponent<TransformComponent>();

		vec2 direction = player_transform.position - transform->position;

		if (direction.x < 0.0f) transform->scale.x =  fabsf(transform->scale.x);
		if (direction.x > 0.0f) transform->scale.x = -fabsf(transform->scale.x);

		if (length(direction) != 0.0f) {
			direction = normalize(direction);
			physics->velocity = direction * static_cast<float>(deltaTime) * speed;
		}
	}

	void OnDie()override {
		destroy_this();
	}

	json Serialize()const override{
		json j;
		FOR_SAVE_FIELD(player);
		FOR_SAVE_FIELD(health);
		FOR_SAVE_VALUE(speed);
		return j;
	}
	void Deserialize(const json& j)override {
		FOR_LOAD_FIELD(player);
		FOR_LOAD_FIELD(health);
		FOR_LOAD_VALUE(speed);
	}

	void OnEditorPanel(IMGUI& imgui)override {
		imgui.EntityPayload("Player", player);
		imgui.DragFloat("Speed", speed);
	}
};