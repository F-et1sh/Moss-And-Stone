#include "forpch.h"
#include "ButtonManager.h"

void FE2D::ButtonManager::Release() {
	if (m_WindowReference != nullptr) {
		m_WindowReference->UnsubscribeOnEvent(Event::EventType::WindowResized,
							m_EventIndices.at(Event::EventType::WindowResized));

		m_WindowReference->UnsubscribeOnEvent(Event::EventType::MouseMoved,
							m_EventIndices.at(Event::EventType::MouseMoved));

		m_WindowReference->UnsubscribeOnEvent(Event::EventType::MouseButtonPressed,
							m_EventIndices.at(Event::EventType::MouseButtonPressed));
		
		m_WindowReference->UnsubscribeOnEvent(Event::EventType::MouseButtonReleased,
							m_EventIndices.at(Event::EventType::MouseButtonReleased));
		
		m_WindowReference = nullptr;
	}
	m_Buttons.clear();
	m_WindowResolution = vec2();
	m_CameraReference = nullptr;
}

void FE2D::ButtonManager::Initialize(Window* window, Camera* camera) {
	this->Release(); // Release Button Manager if it's already Initialized

	if (!window)
		FOR_RUNTIME_ERROR("Failed to Initialize Button Manager\nWindow was nullptr");
	if (!camera)
		FOR_RUNTIME_ERROR("Failed to Initialize Button Manager\nCamera was nullptr");

	// Initialize Event Indices ( indices to delete )
	m_EventIndices.emplace(		 Event::EventType::WindowResized,
		window->SubscribeOnEvent(Event::EventType::WindowResized,       [this](const Event& e) {this->onWindowResized      (e); }));
	
	m_EventIndices.emplace(		 Event::EventType::MouseMoved,		        
		window->SubscribeOnEvent(Event::EventType::MouseMoved,          [this](const Event& e) {this->onMouseMoved         (e); }));
	
	m_EventIndices.emplace(		 Event::EventType::MouseButtonPressed,
		window->SubscribeOnEvent(Event::EventType::MouseButtonPressed,  [this](const Event& e) {this->onMouseButtonPressed (e); }));
	
	m_EventIndices.emplace(		 Event::EventType::MouseButtonReleased,
		window->SubscribeOnEvent(Event::EventType::MouseButtonReleased, [this](const Event& e) {this->onMouseButtonReleased(e); }));
	
	m_WindowResolution = window->getResolution();
	m_WindowReference  = window;
	m_CameraReference  = camera;
}

#pragma region Events Realization

void FE2D::ButtonManager::onWindowResized(const Event& e) {
	// Do not check m_IsActive here
	// m_WindowResolution must be updated

	const FE2D::WindowResized* window_size = static_cast<const FE2D::WindowResized*>(&e);
	m_WindowResolution = window_size->size;
}

void FE2D::ButtonManager::onMouseMoved(const Event& e) {
	if (!m_IsActive) return; // Out from this function if this Button Manager isn't active

	const FE2D::MouseMoved* mouseMoved = static_cast<const FE2D::MouseMoved*>(&e);
	m_CursorPosition = vec2(mouseMoved->position.x - m_WindowResolution.x / 2,
		                   -mouseMoved->position.y + m_WindowResolution.y / 2);
	
	m_CursorPosition /= m_WindowResolution / m_CameraReference->m_VisionSize; // Translate the Cursor Position by the Camera

	bool taken = false; // if the cursor is clicked some button

	for (auto& button : m_Buttons) {
		if (!button->m_IsHitted_Callback) { // Check if the callback exists
			SAY("ERROR : Failed to call a Hitted Callback of Button\nThe Callback is not Initialized");
			continue;
		}

		vec2 button_half_size = abs(button->m_Size) / vec2(2);				       // Get the Halh Size of the Button
		vec2 button_position = button->m_Position - m_CameraReference->m_Position; // Shift the Button by the Camera

		if (m_CursorPosition.x >= button_position.x - button_half_size.x && // Check for Out of left
			m_CursorPosition.x <= button_position.x + button_half_size.x && // Check for Out of bottom
			m_CursorPosition.y >= button_position.y - button_half_size.y && // Check for Out of right
			m_CursorPosition.y <= button_position.y + button_half_size.y) { // Check for Out of top

			taken = true; // register the click

			if (!button->m_IsHitted) { // If the Button wasn't Hitted and now it is
				button->m_IsHitted = true;
				button->m_IsHitted_Callback(button->m_IsHitted);
			}
		}
		else if (button->m_IsHitted) { // If the Button was Hitted and now Released
			button->m_IsHitted = false;
			button->m_IsHitted_Callback(button->m_IsHitted);
		}

		// Call onMouseMoved() function if the Button is UI_Button
		if (auto dragButton = dynamic_cast<UI_Button*>(button)) {
			dragButton->onMouseMoved(m_CursorPosition);
		}

		// exit from the function if the cursor is clicked some other button
		// if you will not do this cursor may click two or more buttons at the same time - that's wrong
		if (taken) return;
	}
}

void FE2D::ButtonManager::onMouseButtonPressed(const Event& e) {
	if (!m_IsActive) return; // Out from this function if this Button Manager isn't active

	const MouseButtonPressed* button_pressed = static_cast<const MouseButtonPressed*>(&e);
	for (auto& button : m_Buttons) {
		if (button_pressed->button == button->m_TriggerButton && // Button's Trigger button pressed
			button->m_IsHitted) {								 // Cursor is hitted the Button

			if (button->m_IsPressed_Callback) { // Check if the callback exists
				button->m_IsPressed_Callback(true);
				button->m_IsPressed = true;
			}
			else
				SAY("ERROR : Failed to call a Pressed Callback of Button");
		}
	}
}

void FE2D::ButtonManager::onMouseButtonReleased(const Event& e) {
	if (!m_IsActive) return; // Out from this function if this Button Manager isn't active

	const MouseButtonReleased* button_pressed = static_cast<const MouseButtonReleased*>(&e);
	for (auto& button : m_Buttons) {
		if (button_pressed->button == button->m_TriggerButton) { // Button's Trigger button pressed
			if (button->m_IsPressed_Callback) { // Check if the callback exists
				button->m_IsPressed_Callback(false);
				button->m_IsPressed = false;
			}
			else
				SAY("ERROR : Failed to call a Released Callback of Button");
		}
	}
}

#pragma endregion