#pragma once

namespace FE2D {
	struct FOR_API IStateNode {
		ResourceID<Animation> animation_id;

		std::string name = "NewStateNode";
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
		
		std::string parameter_name_x; // float
		std::string parameter_name_y; // float
		
		FOR_CLASS_DEFAULT(BlendTreeNode)
		std::unique_ptr<IStateNode> clone() const override { return std::make_unique<BlendTreeNode>(*this); }
	};

	struct FOR_API Trigger {
	private:
		bool used = false;

	public:
		inline void trigger()noexcept { used = true; }
		inline void reset()noexcept { used = false; }

		inline bool is_triggered()const noexcept { return used; }

		FOR_CLASS_DEFAULT(Trigger)

		Trigger(bool is_triggered)noexcept { used = is_triggered; }
	};

	struct FOR_API AnimationParameter {
		std::variant<bool, float, int, Trigger> value;

		FOR_CLASS_DEFAULT(AnimationParameter)

		template<typename T> requires (std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, int> || std::is_same_v<T, Trigger>)
		AnimationParameter(T value) : value(value) {}
	};

	enum class ConditionType { Equals, Greater, Less };

	struct FOR_API AnimationCondition {
		std::string parameter_name;
		ConditionType condition_type = ConditionType::Equals;
		float value = 0.0f;

		FOR_CLASS_DEFAULT(AnimationCondition)
	};

	struct AnimationTransition {
		size_t from_state = 0;
		size_t to_state = 0;
		std::vector<AnimationCondition> conditions;

		FOR_CLASS_DEFAULT(AnimationTransition)

		AnimationTransition(size_t from_state, size_t to_state) : from_state(from_state), to_state(to_state) {}
	};
}