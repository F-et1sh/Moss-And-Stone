#pragma once

namespace FE2D {
	class FOR_API Animator {
	public:
		Animator() = default;
		~Animator() = default;

		//	FOR_API void Release();
		//public:
		//	FOR_API inline void ReserveAnimations(unsigned int new_capacity) { m_AnimationStates.reserve(new_capacity); }
		//	FOR_API inline void AddAnimation(Animation* animation_ref, std::function<bool()> condition) { m_AnimationStates.push_back(AnimationState(animation_ref, condition)); }
		//	FOR_API inline void BindSprite(Sprite* load_sprite) { m_BoundSprite = std::unique_ptr<Sprite>(load_sprite); }
		//public:
		//	FOR_API inline void CreateBlendTree(vec2* current_coord, std::function<bool()> condition) {
		//		m_BlendTrees.push_back(BlendTree(current_coord, condition)); 
		//	}
		//	// Add an Animation to the Last Created Blend Tree
		//	FOR_API inline void BindAnimation(Animation* animation_ref, vec2 coordinates_at_which_the_animation_will_be_played) {
		//		m_BlendTrees[m_BlendTrees.size()-1].add_animation_point(animation_ref, coordinates_at_which_the_animation_will_be_played);
		//	}
		//private:
		//	void ApplyAnimation(BlendTree& blendTree, double load_deltaTime);
		//	void ApplyAnimation(AnimationState& blendTree, double load_deltaTime);
		//public:
		//	FOR_API void Update(double load_deltaTime);
		//private:
		//	std::unique_ptr<Sprite> m_BoundSprite = nullptr;
		//private:
		//	std::vector<AnimationState> m_AnimationStates;
		//	std::vector<BlendTree> m_BlendTrees;
		//private:
		//	float m_CurrentFrame = 0.0f;
	};
}