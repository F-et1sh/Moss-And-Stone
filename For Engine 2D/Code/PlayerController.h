#pragma once

class PlayerController : public ScriptableEntity {
public:
	ComponentField<TransformComponent> transform;
	ComponentField<SpriteComponent> sprite;
	ComponentField<AnimatorComponent> animator;

	vec2 dir = vec2();
	static constexpr float speed = 60;

	vec2 view_dir = vec2();

	PlayerController()
	{
	}
	~PlayerController()
	{
	}

	void OnStart()override {
		transform = this_entity();
		animator = this_entity();
		sprite = this_entity();

		subscribe_to_event(EventType::MouseMoved, [&](const IEvent& e) {
			const MouseMoved& _event = static_cast<const MouseMoved&>(e);
			view_dir = _event.position;
			view_dir.y = m_Scene->getRenderContext().getResolution().y - view_dir.y;
			view_dir  -= m_Scene->getRenderContext().getResolution() / vec2(2);
			});
	}

	void OnUpdate(double deltaTime)override {
		if (is_key_pressed(GLFW_KEY_W)) dir += vec2( 0,  1);
		if (is_key_pressed(GLFW_KEY_S)) dir += vec2( 0, -1);
		if (is_key_pressed(GLFW_KEY_A)) dir += vec2(-1,  0);
		if (is_key_pressed(GLFW_KEY_D)) dir += vec2( 1,  0);

		if (length(dir) != 0.0f) {
			dir = normalize(dir);
			animator->state = AnimatorComponent::State::PLAY;
		}
		else animator->state = AnimatorComponent::State::STOP;

		transform->position += dir * speed * vec2(deltaTime);

		if (view_dir.x > 0) sprite->flip_x = false;
		else				sprite->flip_x = true;

		animator->current_direction = vec2(abs(view_dir.x), view_dir.y);

		dir = {};
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