#pragma once

namespace FE2D {
	class FOR_API Cursor {
	public:
		Cursor() = default;
		~Cursor() = default;

		void Initialize(Window& window, RenderContext& render_context);
		void Render();

		inline vec2 getPosition()const noexcept { return m_MousePosition; }
		
	private:
		Shader m_Shader;
		Texture m_Texture;
		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;

		vec2 m_MousePosition = vec2();

		Window* m_Window = nullptr;
		RenderContext* m_RenderContext = nullptr;
		EventSubscription m_Event_MouseMoved;
	};
}