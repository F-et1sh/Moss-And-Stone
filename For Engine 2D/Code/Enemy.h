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
	PhysicsLayer bullet_layer = 0;

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

	void OnTriggerEnter(Entity entity)override {
		if (entity.HasComponent<NativeScriptComponent>()) {
			auto& entity_script = entity.GetComponent<NativeScriptComponent>();
			
			using T = std::decay_t<decltype(entity_script.instance)>;
			//std::is_base_of_v<IBullet, T> -> ...

			if (entity_script.instance) {
				if (entity.HasComponent<PhysicsComponent>()) {
					auto& entity_physics = entity.GetComponent<PhysicsComponent>();
					if (entity_physics.layer == bullet_layer) {
						health->take_damage(1);
					}
				}
			}
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
		FOR_SAVE_VALUE(bullet_layer);
		return j;
	}
	void Deserialize(const json& j)override {
		FOR_LOAD_FIELD(player);
		FOR_LOAD_FIELD(health);
		FOR_LOAD_VALUE(speed);
		FOR_LOAD_VALUE(bullet_layer);
	}

	void OnEditorPanel(IMGUI& imgui)override {
		imgui.EntityPayload("Player", player);
		imgui.DragFloat("Speed", speed);
		imgui.SelectLayer("Bullet layer", bullet_layer);
	}
};