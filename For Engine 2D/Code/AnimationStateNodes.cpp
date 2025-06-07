#include "forpch.h"
#include "AnimationStateNodes.h"

#undef max

void FE2D::BlendTreeNode::update_current_animation(vec2 load_coords) {
	float shortest_distance = std::numeric_limits<float>::max(); // some big value
	size_t current_anim = 0;
	for (size_t i = 0; i < animation_points.size(); i++) {
		float load_distance = dot(load_coords - animation_points[i].first,
								  load_coords - animation_points[i].first); // use dot() instead of distance()
		if (shortest_distance > load_distance) {
			shortest_distance = load_distance;
			current_anim = i;
		}
	}
	animation_id = animation_points[current_anim].second;
}
