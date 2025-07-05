#pragma once

class Bullet : public ScriptableEntity {

	GENERATED_BODY(Bullet)

public:
	Bullet()
	{
	}
	~Bullet()
	{
	}

	FOR_COMPONENT_FIELD(TransformComponent, transform);

	void OnAwake()override {
		transform = this_entity();
	}

	void OnUpdate(double deltaTime)override {
		destroy_this();
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