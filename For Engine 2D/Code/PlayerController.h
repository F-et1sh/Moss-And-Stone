#pragma once

class PlayerController : public ScriptableEntity {

	GENERATED_BODY(PlayerController)

public:
	PlayerController()
	{
	}
	~PlayerController()
	{
	}

	FOR_COMPONENT_FIELD(TransformComponent, transform);
	FOR_COMPONENT_FIELD(PhysicsComponent, physics);
	FOR_COMPONENT_FIELD(SpriteComponent, sprite);
	FOR_COMPONENT_FIELD(AnimatorComponent, animator);

	vec2 dir = vec2();
	static constexpr float speed = 60;

	vec2 mouse_position = vec2();

	void OnStart()override {
		transform = this_entity();
		physics = this_entity();
		sprite = this_entity();
		animator = this_entity();

		subscribe_to_event(EventType::MouseMoved, [&](const IEvent& e) {
			const MouseMoved& _event = static_cast<const MouseMoved&>(e);
			mouse_position = _event.position;
			mouse_position.y = m_Scene->getRenderContext().getResolution().y - mouse_position.y;
			mouse_position -= m_Scene->getRenderContext().getResolution() / vec2(2);
			});
	}

	void OnUpdate(double deltaTime)override {
		if (is_key_pressed(GLFW_KEY_W)) dir += vec2( 0,  1);
		if (is_key_pressed(GLFW_KEY_S)) dir += vec2( 0, -1);
		if (is_key_pressed(GLFW_KEY_A)) dir += vec2(-1,  0);
		if (is_key_pressed(GLFW_KEY_D)) dir += vec2( 1,  0);

		if (length(dir) != 0.0f) {
			dir = normalize(dir);
		}

		animator->setFloat("DirectionX", abs(mouse_position.x));
		animator->setFloat("DirectionY", mouse_position.y);

		physics->velocity = dir * speed * vec2(deltaTime);

		if (mouse_position.x > 0) sprite->flip_x = false;
		else					  sprite->flip_x = true;

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