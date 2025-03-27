#include "forpch.h"
#include "Button.h"

inline void FE2D::Button::setIsHittedCallback(std::function<void(const bool)> callback) noexcept {
	m_IsHitted_Callback = callback;
}

inline void FE2D::Button::setIsPressedCallback(std::function<void(const bool)> callback) noexcept {
	m_IsPressed_Callback = callback;
}

inline void FE2D::Button::ChangeTriggerButton(int new_button) noexcept {
	m_TriggerButton = new_button;
}
