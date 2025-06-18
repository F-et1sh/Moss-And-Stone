#pragma once

namespace FE2D {
	class FOR_API Cursor {
	public:
		Cursor() = default;
		~Cursor();

		void Initialize(Window& window, RenderContext& render_context);
		void Render();
		
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