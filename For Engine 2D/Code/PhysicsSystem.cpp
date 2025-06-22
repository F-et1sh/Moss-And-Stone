#include "forpch.h"
#include "PhysicsSystem.h"

#undef min
#undef max

void FE2D::PhysicsSystem::Update() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto group = registry.group<PhysicsComponent>(entt::get<TransformComponent>);
	for (auto e_1 : group) {
		Entity entity_1{ e_1, m_Scene };

		auto& transform_1 = entity_1.GetComponent<TransformComponent>();
		auto& physics_1  = entity_1.GetComponent<PhysicsComponent>();

		if (physics_1.is_static) continue;

		mat4 mat_1 = entity_1.GetGlobalTransform();
		vec2 pos_1 = physics_1.position + IMGUI::extractPosition(mat_1);
		vec2 size_1 = physics_1.size;

		for (auto& e_2 : group) {
			if (e_1 == e_2) continue;

			Entity entity_2 = { e_2, m_Scene };

			auto& transform_2 = entity_2.GetComponent<TransformComponent>();
			auto& physics_2 = entity_2.GetComponent<PhysicsComponent>();

			mat4 mat_2 = entity_2.GetGlobalTransform();
			vec2 pos_2 = physics_2.position + IMGUI::extractPosition(mat_2);
			vec2 size_2 = physics_2.size;

			vec2 mtv = GetMTV(pos_1, size_1, pos_2, size_2);
			if (mtv == vec2()) continue;

			physics_1.entity_in = entity_2;
			physics_2.entity_in = entity_1;

			if (physics_1.is_trigger || physics_2.is_trigger) continue;

			transform_1.position -= mtv;
		}
	}
}

bool FE2D::PhysicsSystem::IsCollided(const vec2& posA, const vec2& sizeA, const vec2& posB, const vec2& sizeB) {
	float leftA  = posA.x - abs(sizeA.x / 2);
	float rightA = posA.x + abs(sizeA.x / 2);
	float topA   = posA.y + abs(sizeA.y / 2);
	float downA  = posA.y - abs(sizeA.y / 2);

	float leftB  = posB.x - abs(sizeB.x / 2);
	float rightB = posB.x + abs(sizeB.x / 2);
	float topB   = posB.y + abs(sizeB.y / 2);
	float downB  = posB.y - abs(sizeB.y / 2);

	return (leftA < rightB && rightA > leftB && topA > downB && downA < topB);
}

vec2 FE2D::PhysicsSystem::GetMTV(const vec2& posA, const vec2& sizeA, const vec2& posB, const vec2& sizeB) {
	vec2 delta = posB - posA;
	float dx = delta.x;
	float dy = delta.y;

	float px = (abs(sizeA.x / 2) + abs(sizeB.x / 2)) - abs(dx);
	float py = (abs(sizeA.y / 2) + abs(sizeB.y / 2)) - abs(dy);

	if (px <= 0 || py <= 0) return {};

	vec2 mtv = {};

	if (px < py) mtv = vec2((dx < 0 ? -1 : 1) * px, 0);
	else		 mtv = vec2(0, (dy < 0 ? -1 : 1) * py);

	return mtv;
}