#include "forpch.h"
#include "PhysicsSystem.h"

#include "ProjectVariables.h"

void FE2D::PhysicsSystem::Update() {
	entt::registry& registry = this->m_Scene->getRegistry();
	auto group = registry.group<PhysicsComponent>(entt::get<TransformComponent>);
	
	m_AABBs.reset();

	for (auto e : group) {
		Entity entity{ e, m_Scene };

		auto& physics = entity.GetComponent<PhysicsComponent>();
		physics.entities_in.clear();
		
		auto& transform = entity.GetComponent<TransformComponent>();

		transform.position += physics.velocity;
		physics.velocity /= vec2(1.1);

		mat4 mat = entity.GetGlobalTransform();
		vec2 pos = physics.position + IMGUI::extractPosition(mat);
		vec2 size = physics.size;

		m_AABBs.add({ entity, pos.x - (size.x / 2), pos.x + (size.x / 2) });
	}

	std::sort(m_AABBs.begin(), m_AABBs.end(), [](const AABB& a, const AABB& b) { return a.min_x < b.min_x; });

	for (auto i = m_AABBs.begin(); i != m_AABBs.end(); i++) {

		auto& physics_1 = i->entity.GetComponent<PhysicsComponent>();
		auto& transform_1 = i->entity.GetComponent<TransformComponent>();

		mat4 mat_1 = i->entity.GetGlobalTransform();
		vec2 pos_1 = physics_1.position + IMGUI::extractPosition(mat_1);
		vec2 size_1 = physics_1.size;

		auto j = i;
		j++;
		for (; j != m_AABBs.end(); j++) {
			if (j->min_x > i->max_x) break;

			auto& physics_2 = j->entity.GetComponent<PhysicsComponent>();
			auto& transform_2 = j->entity.GetComponent<TransformComponent>();

			auto& layers = m_ProjectVariables->getPhysicsLayers();
			if (!layers.canCollide(physics_1.layer, physics_2.layer)) continue;

			mat4 mat_2 = j->entity.GetGlobalTransform();
			vec2 pos_2 = physics_2.position + IMGUI::extractPosition(mat_2);
			vec2 size_2 = physics_2.size;

			if (std::abs(pos_1.y - pos_2.y) > (size_1.y + size_2.y) / 2) continue;

			vec2 mtv = GetMTV(pos_1, size_1, pos_2, size_2);
			if (mtv == vec2()) continue;

			physics_1.entities_in.emplace_back(j->entity);
			physics_2.entities_in.emplace_back(i->entity);

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