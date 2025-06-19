#pragma once
#include "ISystemBase.h"

#include "Animation.h"
#include "AnimationStateNodes.h"

namespace FE2D {
	class FOR_API AnimationSystem : public ISystemBase {
	public:
		AnimationSystem() = default;
		~AnimationSystem() = default;

		void Render();

	private:
		static ResourceID<Animation> GetNearAnimation(const std::vector<std::pair<vec2, ResourceID<Animation>>>& animation_points, vec2 current_direction);
	};
}