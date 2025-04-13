#pragma once
#include "Texture.h"

namespace FE2D {
	class FOR_API TransformComponent; // forward declaration
	class FOR_API Camera; // forward declaration
	class FOR_API Window; // forward declaration

	class FOR_API IMGUI {
	public:
		IMGUI() = default;
		~IMGUI() = default;
	public:
		static constexpr const char* GLSL_VERSION = "#version 460";
		static constexpr float DEFAULT_PIXEL_SIZE = 20.0f;
	public:
		void Release();
		void Initialize(Window* window_reference, ResourceManager* resource_manager);

		inline void BeginFrame();
		inline void EndFrame();

		inline void PreviewWindowViewport(size_t x, size_t y, size_t width, size_t height)noexcept { m_PreviewWindowViewport = vec4(x, y, width, height); }
		inline void PreviewWindowSize(const vec2& size)noexcept { m_PreviewWindowSize = size; }
		inline void PreviewWindowPosition(const vec2& position)noexcept { m_PreviewWindowPosition = position; }

		inline void setCamera(Camera& camera)noexcept { m_Camera = &camera; }

		void StartDockSpace();

		void CheckBox   (const std::string& label, bool & value , float columnWidth = 100.0f);
		void DragInt    (const std::string& label, int  & value , float columnWidth = 100.0f);
		void DragFloat  (const std::string& label, float& value , float columnWidth = 100.0f);
		void DragVector2(const std::string& label, vec2 & values, float resetValue = 0.0f, float columnWidth = 100.0f);
		
		std::optional<std::pair<size_t, Texture&>> SelectTexture();

		void TransformControl(TransformComponent& transform);

	private:
		bool DrawGizmoArrow(const vec2& from, const vec2& to, const vec4& color, bool is_dragging, ImDrawList* draw = ImGui::GetForegroundDrawList());
		bool DrawGizmoRect(const vec2& position, const vec2& size, const vec4& color, bool is_dragging, ImDrawList* draw = ImGui::GetForegroundDrawList());

		Window* m_Window = nullptr;
		size_t m_EventIndex = 0; // to unsubscribe on WindowResized event

		bool m_IsDraggingX = false;
		bool m_IsDraggingY = false;
		bool m_IsDraggingRect = false;

		bool m_CtrlWasPressed = false;
		vec2 m_InitialMousePosition = vec2();
		vec2 m_InitialTransformPosition = vec2();

		vec4 m_PreviewWindowViewport = vec4();

		vec2 m_PreviewWindowSize = vec2();
		vec2 m_PreviewWindowPosition = vec2();

	private:
		ResourceManager* m_ResourceManager = nullptr;
		Camera* m_Camera = nullptr;
	};
}