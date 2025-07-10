#pragma once

class Bullet : public ScriptableEntity {

	GENERATED_BODY(Bullet)

	void OnRegisterFields()override {
		FOR_REGISTER_FIELD(transform);
		FOR_REGISTER_FIELD(physics);
	}

public:
	ComponentField<TransformComponent> transform;
	ComponentField<PhysicsComponent> physics;

	Bullet()
	{
	}
	~Bullet()
	{
	}

	void OnAwake()override {
		transform = this_entity();
		physics = this_entity();
	}

	void OnUpdate(double deltaTime)override {
		if (fabsf(physics->velocity.x) < 0.1f && fabsf(physics->velocity.y) < 0.1f) {
			destroy_this();
		}
	}

	void OnTriggerEnter(Entity entity)override {
		if (entity.HasComponent<PhysicsComponent>()) {
			
			auto& physics = entity.GetComponent<PhysicsComponent>();
			
			if (get_project_variables().getPhysicsLayers().get_name_by_mask(physics.layer) == "Enemy") {
				
				if (!entity.HasComponent<E_FreezeComponent>())
					entity.AddComponent<E_FreezeComponent>(1.0f);
				else {
					entity.GetComponent<E_FreezeComponent>().duration += 1.0f;
				}
			}
		}
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