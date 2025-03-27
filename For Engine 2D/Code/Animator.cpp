#include "forpch.h"
#include "Animator.h"

//void Animator::Release() {
//	m_AnimationStates.clear();
//	m_BlendTrees.clear();
//}
//
//void Animator::ApplyAnimation(BlendTree& blendTree, double load_deltaTime) {
//	Animation* current_animation = blendTree.get_current_animation();
//
//	m_CurrentFrame += static_cast<float>(load_deltaTime * static_cast<double>(current_animation->get_speed()));
//
//	if (m_CurrentFrame > current_animation->get_FramesCount() && current_animation->get_repeated())
//		m_CurrentFrame = 0;
//
//	m_BoundSprite->m_TexPath = current_animation->get_TexturePath();
//	m_BoundSprite->m_TexCoords = current_animation->get_key(int(m_CurrentFrame)).texCoords;
//	m_BoundSprite->m_IsFlipped = current_animation->get_key(int(m_CurrentFrame)).isFlipped;
//}
//void Animator::ApplyAnimation(AnimationState& state, double load_deltaTime) {
//	Animation* current_animation = state.animation;
//
//	m_CurrentFrame += static_cast<float>(load_deltaTime * static_cast<double>(current_animation->get_speed()));
//
//	if (m_CurrentFrame > current_animation->get_FramesCount() && current_animation->get_repeated())
//		m_CurrentFrame = 0;
//
//	m_BoundSprite->m_TexPath = current_animation->get_TexturePath();
//	m_BoundSprite->m_TexCoords = current_animation->get_key(int(m_CurrentFrame)).texCoords;
//	m_BoundSprite->m_IsFlipped = current_animation->get_key(int(m_CurrentFrame)).isFlipped;
//}
//
//void Animator::Update(double load_deltaTime) {
//	for (auto& blendTree : m_BlendTrees) {
//		if (blendTree.condition()) {
//			ApplyAnimation(blendTree, load_deltaTime);
//			break;
//		}
//	}
//	for (auto& state : m_AnimationStates) {
//		if (state.condition()) {
//			ApplyAnimation(state, load_deltaTime);
//			break;
//		}
//	}
//}