#pragma once
#include "ISystemBase.h"

namespace FE2D {
	class FOR_API PhysicsSystem : public ISystemBase {
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() = default;

		void Update();

	private:
		static bool IsCollided(const vec2& posA, const vec2& sizeA, const vec2& posB, const vec2& sizeB);
	};
}