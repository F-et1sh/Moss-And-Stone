#pragma once
#include "Animation.h"

namespace FE2D {
	struct FOR_API AnimationState {
	public:
		Animation* animation = nullptr;
		std::function<bool()> condition;

	public:
		AnimationState(Animation* animation, std::function<bool()> condition) : animation(animation), condition(condition) {}

		AnimationState() = default;
		~AnimationState() = default;
	};
}