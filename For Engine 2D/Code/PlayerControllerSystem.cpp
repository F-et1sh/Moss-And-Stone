#include "forpch.h"
#include "PlayerControllerSystem.h"

#undef max

void FE2D::PlayerControllerSystem::Release() {

}

void FE2D::PlayerControllerSystem::Initialize() {
	
}

json FE2D::PlayerControllerSystem::Serialize() const {
	json j;
	if (m_Player) j["Player"] = m_Player.GetUUID().ToString();
	return j;
}

void FE2D::PlayerControllerSystem::Deserialize(const json& j) {
	FE2D::UUID player_uuid = FE2D::UUID(j["Player"].get<std::string>());
	m_Player = m_Scene->GetEntityByUUID(player_uuid);
}

void FE2D::PlayerControllerSystem::Update() {
	if (!m_Player) return;

	auto& transform = m_Player.GetComponent<TransformComponent>();
	auto& sprite	= m_Player.GetComponent<SpriteComponent>();
	auto& player	= m_Player.GetComponent<PlayerComponent>();
	auto& animator	= m_Player.GetComponent<CharacterAnimatorComponent>();
	auto& velocity	= m_Player.GetComponent<VelocityComponent>();

	vec2 move_direction = vec2();
	constexpr float speed = 100;

	if (glfwGetKey(m_Window->reference(), GLFW_KEY_W)) move_direction += vec2( 0,  1);
	if (glfwGetKey(m_Window->reference(), GLFW_KEY_S)) move_direction += vec2( 0, -1);
	if (glfwGetKey(m_Window->reference(), GLFW_KEY_D)) move_direction += vec2( 1,  0);
	if (glfwGetKey(m_Window->reference(), GLFW_KEY_A)) move_direction += vec2(-1,  0);

	const auto& view_direction = GetViewDirection();
	switch (view_direction) {
	case ViewDirections::UP:
		animator.setCurrentAnimation("WalkUp");
		break;
	case ViewDirections::DOWN:
		animator.setCurrentAnimation("WalkDown");
		break;
	case ViewDirections::RIGHT_UP:
		animator.setCurrentAnimation("WalkRightUp");
		transform.scale.x = fabsf(transform.scale.x);
		break;
	case ViewDirections::RIGHT_DOWN:
		animator.setCurrentAnimation("WalkRightDown");
		transform.scale.x = fabsf(transform.scale.x);
		break;
	case ViewDirections::LEFT_UP:
		animator.setCurrentAnimation("WalkRightUp");
		transform.scale.x = -fabsf(transform.scale.x);
		break;
	case ViewDirections::LEFT_DOWN:
		animator.setCurrentAnimation("WalkRightDown");
		transform.scale.x = -fabsf(transform.scale.x);
		break;

	default:
		break;
	}

	if (length(move_direction) != 0.0f) {

		move_direction = glm::normalize(move_direction);
		animator.time += m_Window->getDeltaTime();
	}
	else {
		animator.time = 0.0f;
	}

	velocity.velocity = move_direction * speed;
}

void FE2D::PlayerControllerSystem::OnPropertiesPanel(IMGUI& imgui) {
	ImGui::Begin("PlayerController System");

	std::string str = m_Player ? m_Player.GetComponent<TagComponent>().tag : "None";
	ImGui::Button(str.c_str());
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAGGING")) {
			m_Player = *static_cast<Entity*>(payload->Data);

			/* check if needed components are exist in the player */
			this->CheckComponent<PlayerComponent>();
			this->CheckComponent<SpriteComponent>();
			this->CheckComponent<CharacterAnimatorComponent>();
			this->CheckComponent<VelocityComponent>();

			/* check if needed animations are exitst in the player */
			auto& animator = m_Player.GetComponent<CharacterAnimatorComponent>();
			this->CheckAnimation(animator, "WalkUp");
			this->CheckAnimation(animator, "WalkDown");
			this->CheckAnimation(animator, "WalkRightUp");
			this->CheckAnimation(animator, "WalkRightDown");

		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

FE2D::PlayerControllerSystem::ViewDirections FE2D::PlayerControllerSystem::GetViewDirection() {
	if constexpr (m_ViewDirections.empty())
		FOR_RUNTIME_ERROR("Player has no view directions");

	double mouse_x = 0.0f;
	double mouse_y = 0.0f;
	
	glfwGetCursorPos(m_Window->reference(), &mouse_x, &mouse_y);

	vec2 mouse_pos = vec2(mouse_x, mouse_y);
	
	mouse_pos /= m_RenderContext->getResolution();
	
	mouse_pos.y = 1.0f - mouse_pos.y;
	
	mouse_pos -= vec2(0.5f);
	mouse_pos *= vec2(2);
	
	mouse_pos = normalize(mouse_pos);

	float best_dot = -2.0f;
	size_t result = 0;

	for (size_t i = 0; i < m_ViewDirections.size(); i++) {
		vec2 dir = normalize(m_ViewDirections[i].first);
		float dot_product = dot(mouse_pos, dir);

		if (dot_product > best_dot) {
			best_dot = dot_product;
			result = i;
		}
	}

	return m_ViewDirections[result].second;
}
