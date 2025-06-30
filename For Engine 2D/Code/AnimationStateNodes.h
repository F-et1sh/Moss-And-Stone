#pragma once

namespace FE2D {
	struct FOR_API IStateNode {
		ResourceID<Animation> animation_id;

		inline void set_animation(ResourceID<Animation> id)noexcept { animation_id = id; }
		inline ResourceID<Animation> get_animation()const noexcept { return animation_id; }

		std::string name;
		float duration = 0.0f;
		bool looping = false;

		virtual std::unique_ptr<IStateNode> clone() const = 0;

		IStateNode() = default;
		virtual ~IStateNode() = default;
	};

	struct FOR_API AnimationStateNode : public IStateNode {
		FOR_CLASS_DEFAULT(AnimationStateNode)
		std::unique_ptr<IStateNode> clone() const override { return std::make_unique<AnimationStateNode>(*this); }
	};

	struct FOR_API BlendTreeNode : public IStateNode {
		void update_current_animation(vec2 load_coords);
	
		using AnimationPoint = std::pair<vec2, ResourceID<Animation>>;
		std::vector<AnimationPoint> animation_points;
		
		FOR_CLASS_DEFAULT(BlendTreeNode)
		std::unique_ptr<IStateNode> clone() const override { return std::make_unique<BlendTreeNode>(*this); }
	};

	enum class ParameterType { Bool, Float, Int };

	struct AnimationParameter {
		std::string name;
		std::variant<bool, float, int> value;

		FOR_CLASS_DEFAULT(AnimationParameter)

		template<typename T> requires (std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, int>)
		AnimationParameter(std::string n, T v) : name(std::move(n)), value(v) {}
	};

	enum class ConditionType { Equals, Greater, Less };

	struct AnimationCondition {
		std::string parameter_name;
		ConditionType condition = ConditionType::Equals;
		float value = 0.0f;

		FOR_CLASS_DEFAULT(AnimationCondition)
	};

	struct AnimationTransition {
		std::string from_state;
		std::string to_state;
		std::vector<AnimationCondition> conditions;

		FOR_CLASS_DEFAULT(AnimationTransition)
	};
}