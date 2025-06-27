#pragma once
#include "ISystemBase.h"

namespace FE2D {
	class FOR_API PhysicsSystem : public ISystemBase {
	public:
		struct FOR_API AABB {
			Entity entity;
			float min_x = 0.0f;
			float max_x = 0.0f;

			AABB(Entity entity, float min_x, float max_x) : entity(entity), min_x(min_x), max_x(max_x) {}

			AABB() = default;
			~AABB() = default;
		};
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() = default;

		void Update();

	private:
		FE2D::dynamic_array<AABB, 250> m_AABBs;

		static bool IsCollided(const vec2& posA, const vec2& sizeA, const vec2& posB, const vec2& sizeB);
		static vec2 GetMTV(const vec2& posA, const vec2& sizeA, const vec2& posB, const vec2& sizeB);
	};
}