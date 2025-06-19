#include "forpch.h"
#include "Cursor.h"

void FE2D::Cursor::Initialize(Window& window, RenderContext& render_context) {
	m_Window = &window;
	m_RenderContext = &render_context;

	m_Window->SubscribeToEvent(m_Event_MouseMoved, EventType::MouseMoved, [&](const IEvent& e) {
		const MouseMoved& _event = static_cast<const MouseMoved&>(e);
		m_MousePosition = _event.position;
		m_MousePosition.y = m_RenderContext->getResolution().y - m_MousePosition.y;
		m_MousePosition  -= m_RenderContext->getResolution() / vec2(2);
		});

	m_VertexArray.Create();
	m_VertexArray.Bind();

	m_VertexBuffer.Create();
	m_VertexBuffer.Bind();

	constexpr static float load_vertices[] = {
		-0.5f,  0.5f,
		-0.5f, -0.5f,
		 0.5f,  0.5f,
		 0.5f, -0.5f,
	};

	m_VertexBuffer.BufferData(sizeof(load_vertices), load_vertices, GL_STATIC_DRAW);

	m_VertexArray.EnableAttrib(0);
	m_VertexArray.AttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (const void*)0);

	m_VertexBuffer.Unbind();
	m_VertexArray.Unbind();

	m_Shader.Initialize(FOR_PATH.get_shaders_path() / "Cursor Default" / "CursorDefault");

	m_Texture.LoadFromFile(FOR_PATH.get_cursors_path() / "CursorQuad.png");
}

void FE2D::Cursor::Render() {
	m_Shader.Bind();
	m_Texture.Bind();

	mat4 matrix = mat4(1.0f);
	matrix = translate(matrix, vec3(m_MousePosition, m_RenderContext->getDepth()));
	matrix = scale(matrix, vec3(m_Texture.getSize(), 1.0f));
	
	m_Shader.setUniformMat4("u_Transform", m_RenderContext->getProjection() * matrix);

	m_VertexArray.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	m_VertexArray.Unbind();

	m_Texture.Unbind();
	m_Shader.Unbind();
}