#pragma once

class IPickable : public ScriptableEntity {

	GENERATED_BODY(IPickable)

	void OnRegisterFields()override {}

public:
	IPickable() = default;
	~IPickable() = default;

	void OnStart()override {
		if (!this_entity().HasComponent<PhysicsComponent>())
			this_entity().AddComponent<PhysicsComponent>();
	}
};