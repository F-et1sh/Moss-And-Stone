#include "forpch.h"
#include "PlayerControllerSystem.h"

void FE2D::PlayerControllerSystem::Release()
{
}

void FE2D::PlayerControllerSystem::Initialize()
{
}

void FE2D::PlayerControllerSystem::Update() {
	if (!m_Player) return;

	auto& transform = m_Player.GetComponent<TransformComponent>();
	auto& sprite = m_Player.GetComponent<SpriteComponent>();
	auto& player = m_Player.GetComponent<PlayerComponent>();
	auto& animator = m_Player.GetComponent<CharacterAnimatorComponent>();
	auto& velocity = m_Player.GetComponent<VelocityComponent>();

	vec2 dir = vec2();
	constexpr float speed = 600;

	if (glfwGetKey(m_Window->reference(), GLFW_KEY_W)) dir += vec2(0, 1);
	if (glfwGetKey(m_Window->reference(), GLFW_KEY_S)) dir += vec2(0, -1);
	if (glfwGetKey(m_Window->reference(), GLFW_KEY_D)) dir += vec2(1, 0);
	if (glfwGetKey(m_Window->reference(), GLFW_KEY_A)) dir += vec2(-1, 0);

	if (length(dir) != 0.0f) {
		dir = glm::normalize(dir);
	}

	velocity.velocity = dir * speed;

	if (dir.x > 0) {
		if (dir.y > 0)
			animator.current_animation = animator.animations.at("WalkRightUp");
		else
			animator.current_animation = animator.animations.at("WalkRightDown");
		sprite.flip_x = false;
	}
	else if (dir.x < 0) {
		if (dir.y > 0)
			animator.current_animation = animator.animations.at("WalkRightUp");
		else
			animator.current_animation = animator.animations.at("WalkRightDown");
		sprite.flip_x = true;
	}
	else {
		if (dir.y > 0)
			animator.current_animation = animator.animations.at("WalkUp");
		else
			animator.current_animation = animator.animations.at("WalkDown");
	}
}

void FE2D::PlayerControllerSystem::OnPropertiesWindow() {
	ImGui::Begin("PlayerController System");

	std::string str = m_Player ? m_Player.GetComponent<TagComponent>().tag : "None";
	ImGui::Button(str.c_str());
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAGGING")) {
			m_Player = *static_cast<Entity*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}
