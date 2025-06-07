#pragma once

namespace FE2D {
	struct FOR_API IStateNode {
		IStateNode() = default; 
		virtual ~IStateNode() = default;

		inline void set_animation(ResourceID<Animation> id)noexcept { animation_id = id; }
		inline ResourceID<Animation> get_animation()const noexcept { return animation_id; }

		void set_condition(const std::function<bool()>& load_condition) noexcept { condition = load_condition; }
		inline void set_condition(std::function<bool()>&& load_condition) noexcept { condition = std::move(load_condition); }
	
	protected:
		ResourceID<Animation> animation_id;
		std::function<bool()> condition;
	};

	struct FOR_API AnimationStateNode : public IStateNode {
		FOR_CLASS_DEFAULT(AnimationStateNode)
	};

	struct FOR_API BlendTreeNode : public IStateNode {
		FOR_CLASS_DEFAULT(BlendTreeNode)
	
		inline void add_animation_point(vec2 load_coords, ResourceID<Animation> load_anim) { animation_points.emplace_back(load_coords, load_anim); }
		void update_current_animation(vec2 load_coords);
	
	private:
		using AnimationPoint = std::pair<vec2, ResourceID<Animation>>;
		std::vector<AnimationPoint, boost::fast_pool_allocator<AnimationPoint>> animation_points;
	};
}