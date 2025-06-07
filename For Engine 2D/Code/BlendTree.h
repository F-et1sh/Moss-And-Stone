#pragma once
#include "Animation.h"

namespace FE2D {
	struct FOR_API BlendTree {
	public:
		std::function<bool()> condition;
	private:
		vec2* current_coord = nullptr;
	private:
		struct FOR_API AnimationPoint {
			Animation* animation = nullptr;
			vec2 coords = vec2();

			AnimationPoint(Animation* animation, vec2 coords) : animation(animation), coords(coords) {}

			AnimationPoint() = default;
			~AnimationPoint() = default;
		};
		dynamic_array<AnimationPoint> animationPoints;
	public:
		inline void add_animation_point(Animation* load_anim, vec2 load_coords) {
			animationPoints.add(AnimationPoint(load_anim, load_coords));
		}
		Animation* get_current_animation() {
			float shortest_distance = FLT_MAX; // some big value
			unsigned int current_anim = 0;
			for (unsigned int i = 0; i < animationPoints.size(); i++) {
				float load_distance = dot(*current_coord - animationPoints[i].coords, 
										  *current_coord - animationPoints[i].coords); // use dot() instead of distance()
				if (shortest_distance > load_distance) {
					shortest_distance = load_distance;
					current_anim = i;
				}
			}
			return animationPoints[current_anim].animation;
		}
	public:
		BlendTree(vec2* current_coord, std::function<bool()> condition) : current_coord(current_coord), condition(condition) {}

		BlendTree() = default;
		~BlendTree() = default;
	};
}