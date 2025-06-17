#pragma once
#include "Camera.h"

namespace FE2D {
	// resolution of the game
	constexpr static inline vec2 LOGICAL_RESOLUTION = { 1920, 1080 };

	class FOR_API RenderContext {
	public:
		RenderContext() = default;
		~RenderContext() = default;

		void Initialize();
		void Initialize(Window& window);

		void BindCamera(Camera& camera) noexcept { m_Camera = &camera; }
		Camera* getCamera()const noexcept { return m_Camera; }

		inline const vec2& getResolution()const noexcept { return m_Resolution; }

		inline const mat4& getView() const noexcept { return m_Camera->getView(); }
		inline const mat4& getProjection()const noexcept { return m_Projection; }
		inline mat4 getViewProjection() const noexcept { return m_Projection * m_Camera->getView(); }

		inline void setViewport(const vec4& viewport)noexcept { m_Viewport = viewport; GLFW::setViewport(m_Viewport); }
		inline const vec4& getViewport()const noexcept { return m_Viewport; }
	private: 
		void UpdateWindowResolution(const vec2& resolution)noexcept;
		void UpdateProjection()noexcept;

	private:
		Camera* m_Camera = nullptr;

		vec4 m_Viewport = vec4();

		mat4 m_Projection = mat4();
		vec2 m_Resolution = vec2();

	private:
		void SubscribeOnEvents(Window& window);

		Window* m_Window = nullptr;
		EventSubscription m_Event_WindowResized;

		friend class Window;
	};
}