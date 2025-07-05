#pragma once
#include "Bullet.h"

class PlayerController : public ScriptableEntity {

	GENERATED_BODY(PlayerController)

	void OnRegisterFields()override {
		FOR_REGISTER_FIELD(transform);
		FOR_REGISTER_FIELD(physics);
		FOR_REGISTER_FIELD(sprite);
		FOR_REGISTER_FIELD(animator);

		FOR_REGISTER_FIELD(shotgun);
	}

public:
	PlayerController()
	{
	}
	~PlayerController()
	{
	}

	ComponentField<TransformComponent> transform;
	ComponentField<PhysicsComponent> physics;
	ComponentField<SpriteComponent> sprite;
	ComponentField<AnimatorComponent> animator;

	EntityField shotgun;

	ResourceID<Prefab> bullet;

	float rotation = 0.0f;

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

		if (is_key_pressed(GLFW_KEY_R)) {
			auto& shotgun_animator = shotgun->GetComponent<AnimatorComponent>();
			shotgun_animator.callTrigger("IsReloading");
		}

		if (is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
			auto& shotgun_animator = shotgun->GetComponent<AnimatorComponent>();
			shotgun_animator.callTrigger("IsShooting");

			Entity created_bullet = this->get_resource_manager().GetResource(bullet).CreateEntity(this->get_scene());
			created_bullet.GetComponent<TransformComponent>().position = transform->position;

			if (created_bullet.HasComponent<PhysicsComponent>()) {
				auto& bullet_physics = created_bullet.GetComponent<PhysicsComponent>();
				bullet_physics.velocity = mouse_position / vec2(50);
			}
		}

		auto& shotgun_transform = shotgun->GetComponent<TransformComponent>();
		auto& shotgun_sprite = shotgun->GetComponent<SpriteComponent>();
		rotation = glm::degrees(atan2(mouse_position.x, mouse_position.y)) - 90;

		if (rotation < -90) {
			rotation += 190;
			shotgun_sprite.flip_x = true;
		}
		else shotgun_sprite.flip_x = false;

		shotgun_transform.rotation = rotation;

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
		FOR_SAVE_FIELD(shotgun);
		FOR_SAVE_RESOURCE(bullet);
		return j;
	}

	void Deserialize(const json& j)override {
		FOR_LOAD_FIELD(shotgun);
		FOR_LOAD_RESOURCE(bullet);
	}

	void OnEditorPanel(IMGUI& imgui)override {
		imgui.EntityPayload("Shotgun", shotgun);
		imgui.DragVector2("mouse_pos", mouse_position);
		imgui.DragFloat("rotation", rotation);
		imgui.SelectPrefab(bullet);
	}
};