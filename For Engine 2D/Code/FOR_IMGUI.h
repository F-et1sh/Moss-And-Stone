#pragma once
#include "Texture.h"
#include "Animation.h"
#include "Components.h"
#include "ComponentField.h"
#include "EventSubscription.h"

namespace FE2D {
	/* forward declarations */
	class TransformComponent;
	class ColliderComponent;
	class Camera;
	class Window;
	class RenderContext;

	class FOR_API IMGUI {
	public:
		IMGUI() = default;
		~IMGUI() = default;
	public:
		inline static constexpr const char* GLSL_VERSION = "#version 460";
		inline static const char* DEFAULT_FONT = "open_sans.ttf";
		inline static constexpr float DEFAULT_PIXEL_SIZE = 20.0f;
	public:
		void Release();
		void Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager);

		void BeginFrame();
		void EndFrame();

		inline void PreviewWindowPosition(const vec2& position)noexcept { m_PreviewWindowPosition = position; }
		inline void PreviewWindowSize(const vec2& size)noexcept { m_PreviewWindowSize = size; }
		inline void PreviewImagePosition(const vec2& position)noexcept { m_PreviewImagePosition = position; }

		void StartDockSpace();

		void CheckBox(const std::string& label, bool& value, float columnWidth = 100.0f);
		void DragInt(const std::string& label, int& value, float columnWidth = 100.0f);
		void DragFloat(const std::string& label, float& value, float columnWidth = 100.0f);
		void DragVector2(const std::string& label, vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);

		void SelectTexture(ResourceID<Texture>& id);
		void SelectAnimation(ResourceID<Animation>& id);
		void DrawAnimation(ResourceID<Animation>& id, ImVec2 sprite_size = ImVec2(100, 100));

		void TransformControl(Entity entity);
		void ColliderControl(TransformComponent& transform, ColliderComponent& collider);

		inline bool IsAnyGizmoHovered()const noexcept { return m_IsAnyGizmoHovered; }

		template<typename T>
		inline void ComponentPayload(ComponentField<T>& component_field) {
			if (component_field.entity) {
				std::string name = component_field.entity.GetComponent<TagComponent>().tag;
				ImGui::Button(name.c_str());
			}
			else {
				std::string name = typeid(T).name();
				ImGui::Button(name.c_str());
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAGGING")) {
					auto entity = *static_cast<Entity*>(payload->Data);
					if (entity.HasComponent<T>())
						component_field.entity = entity;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text(typeid(T).name());
		}
		void EntityPayload(Entity& load_entity) {
			if (load_entity) {
				std::string name = load_entity.GetComponent<TagComponent>().tag;
				ImGui::Button(name.c_str());
			}
			else {
				std::string name = "Entity";
				ImGui::Button(name.c_str());
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAGGING")) {
					auto entity = *static_cast<Entity*>(payload->Data);
					load_entity = entity;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text("Entity");
		}
	public:
		Window* getWindow()const noexcept { return m_Window; }
		RenderContext* getRenderContext()const noexcept { return m_RenderContext; }
		ResourceManager* getResourceManager()const noexcept { return m_ResourceManager; }

	public:
		bool IsAnyTransformGizmoDragging()const noexcept { return m_IsDraggingX || m_IsDraggingY || m_IsDraggingRect; }
		bool IsAnyColliderGizmoDragging()const noexcept { return m_IsDraggingColliderLeft || m_IsDraggingColliderRight || m_IsDraggingColliderTop || m_IsDraggingColliderBottom; }

		bool DrawGizmoArrow(const vec2& from, const vec2& to, const vec4& color, bool is_dragging, ImDrawList* draw = ImGui::GetForegroundDrawList());
		bool DrawGizmoRect(const vec2& position, const vec2& size, const vec4& color, bool is_dragging, ImDrawList* draw = ImGui::GetForegroundDrawList());

	private:
		vec2 GetWorldPosition(const mat4& matrix);
		static vec2 extractScale(const glm::mat4& matrix);

		ImDrawList* GetPreviewWindowDrawList();

	private:
		Window* m_Window = nullptr;
		EventSubscription m_Event_WindowResized;

		RenderContext* m_RenderContext = nullptr;

		bool m_IsAnyGizmoHovered = false;

	private:
		bool m_IsDraggingColliderLeft = false;
		bool m_IsDraggingColliderRight = false;
		bool m_IsDraggingColliderTop = false;
		bool m_IsDraggingColliderBottom = false;

	private:
		bool m_IsDraggingX = false;
		bool m_IsDraggingY = false;
		bool m_IsDraggingRect = false;

		bool m_CtrlWasPressed = false;
		vec2 m_InitialMousePosition = vec2();
		vec2 m_InitialTransformPosition = vec2();

		vec2 m_PreviewWindowPosition = vec2();
		vec2 m_PreviewWindowSize = vec2();
		vec2 m_PreviewImagePosition = vec2();

	private:
		ResourceManager* m_ResourceManager = nullptr;
	};
}