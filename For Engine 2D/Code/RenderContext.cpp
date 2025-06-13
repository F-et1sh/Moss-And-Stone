#include "forpch.h"
#include "RenderContext.h"

#include "Window.h"

void FE2D::RenderContext::Initialize() {
	this->Release();

	this->UpdateWindowResolution(LOGICAL_RESOLUTION);
	this->UpdateProjection();

	this->setViewport(vec4(0, 0, LOGICAL_RESOLUTION));
}

void FE2D::RenderContext::Initialize(Window& window) {
	this->Release();

	this->UpdateWindowResolution(window.getResolution());
	this->UpdateProjection();

	this->setViewport(vec4(0, 0, window.getResolution()));
	
	this->SubscribeOnEvents(window);
}

void FE2D::RenderContext::UpdateWindowResolution(const vec2& resolution)noexcept {
	m_Resolution = resolution;
}

void FE2D::RenderContext::UpdateProjection() noexcept {
	const float aspect = m_Resolution.x / m_Resolution.y;
	const float logical_half_height = LOGICAL_RESOLUTION.y / 2.0f;
	const float logical_half_width = logical_half_height * aspect;

	m_Projection = ortho(-logical_half_width , logical_half_width,
						 -logical_half_height, logical_half_height,
						 -1000.0f, 1000.0f);
}

void FE2D::RenderContext::SubscribeOnEvents(Window& window) {
	m_EventIndex = window.SubscribeOnEvent(Event::EventType::WindowResized, [&](const Event& e) {
		const WindowResized* _event = static_cast<const WindowResized*>(&e);
		this->UpdateWindowResolution(_event->size);
		this->UpdateProjection();

		this->setViewport(vec4(0, 0, _event->size));
		});
	m_Window = &window;
}

void FE2D::RenderContext::UnsubscribeOnEvents() {
	if (!m_Window) return;

	m_Window->UnsubscribeOnEvent(Event::EventType::WindowResized, m_EventIndex);
	m_Window = nullptr;
}
