#pragma once

class PlayerController : public ScriptableEntity {
public:
	ComponentField<TransformComponent> transform;
	ComponentField<AnimatorComponent> animator;

	vec2 dir = vec2();
	static constexpr float speed = 60;

	PlayerController()
	{
	}
	~PlayerController()
	{
	}

	void OnStart()override {
		transform = this_entity();
		animator = this_entity();
	}

	void OnUpdate(double deltaTime)override {
		if (is_key_pressed(GLFW_KEY_W)) dir += vec2(0, 1);
		if (is_key_pressed(GLFW_KEY_S)) dir += vec2(0, -1);
		if (is_key_pressed(GLFW_KEY_A)) dir += vec2(-1, 0);
		if (is_key_pressed(GLFW_KEY_D)) dir += vec2(1, 0);

		if (length(dir) != 0.0f)
			dir = normalize(dir);

		transform->position += dir * speed * vec2(deltaTime);

		dir = {};

		//animator->current_animation = animator->animations.begin()->second;
		//animator->time += deltaTime;
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