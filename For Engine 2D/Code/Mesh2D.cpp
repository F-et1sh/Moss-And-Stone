#include "forpch.h"
#include "Mesh2D.h"

void FE2D::Mesh2D::Release() {
	m_DefaultShader.Release();
	m_VertexArray.Delete();
	m_VertexBuffer.Delete();
	m_DrawMode = 0;
	m_VerticesCount = 0;

	m_VertexData.clear();
}

void FE2D::Mesh2D::Initialize(const vec2* vertices, const unsigned int vertices_first, const unsigned int vertices_count, const int usage, const unsigned int draw_mode) {
	this->Release();
	
	// Vertex Shader
	{
		constexpr const char* vertex_shader =
			R"(
			#version 460
			layout (location = 0) in vec2 a_Vertex;
			
			out vec4 i_Color;
			
			uniform mat4 u_Transform;
			uniform vec4 u_Color;
			
			uniform mat4 u_Camera;
			
			void main() {
				gl_Position = u_Camera * u_Transform * vec4(a_Vertex, 0.0f, 1.0f);
				i_Color = u_Color;
			}
		)";

		m_DefaultShader.loadFromSource(vertex_shader, FE2D::Shader::VERTEX);
	}

	// Fragment Shader
	{
		constexpr const char* fragment_shader =
			R"(
			#version 460
			out vec4 fragColor;
			
			in vec4 i_Color;
			
			void main() {
				fragColor = i_Color;
			}
		)";

		m_DefaultShader.loadFromSource(fragment_shader, FE2D::Shader::FRAGMENT);
	}

	// Link the Shader
	m_DefaultShader.LinkShader();


	m_VertexArray.Create();
	m_VertexArray.Bind();

	m_VertexBuffer.Create();
	m_VertexBuffer.Bind();

	m_VertexBuffer.BufferData(sizeof(vec2) * vertices_count, vertices, usage);
	
	m_VertexArray.EnableAttrib(0);
	m_VertexArray.AttribPointer(0, 2, GL_FLOAT, false, sizeof(vec2), nullptr);
	
	m_VertexArray.Unbind();
	m_VertexBuffer.Unbind();
	
	m_DrawMode = draw_mode;
	m_VerticesFirst = vertices_first;
	m_VerticesCount = vertices_count;

	m_VertexData.clear();
	m_VertexData.add(vertices, vertices_count);
}

void FE2D::Mesh2D::UpdateMesh(const vec2* vertices, const unsigned int offset, const unsigned int size) {
	m_VertexBuffer.Bind();
	m_VertexBuffer.BufferSubData(offset, size, vertices);
	m_VertexBuffer.Unbind();

	m_VertexData.reset();
	m_VertexData.add(vertices, size/sizeof(vec2));
}

inline void FE2D::Mesh2D::draw() const {
	mat4 load_matrix = mat4(1.0f);

	// Scaling
	{
		load_matrix = scale(load_matrix, vec3(m_Size, 1.0f));
	}
	// Rotation
	{
		// Set Origin
		load_matrix = translate(load_matrix, vec3(-m_Origin.x - m_Position.x, -m_Origin.y - m_Position.y, 0.0f));
		// Rotate
		load_matrix = glm::rotate(load_matrix, glm::radians(m_Rotation), vec3(0.0f, 0.0f, 1.0f));
		// Reset Origin
		load_matrix = translate(load_matrix, vec3(m_Origin.x + m_Position.x, m_Origin.y + m_Position.y, 0.0f));
	}
	// Translation
	{
		load_matrix = translate(load_matrix, vec3(m_Position,            // Position
			((m_AutoSort * (-m_Position.y * m_Size.y / 2)) + // Auto-Sorting
				m_Layer)));                                  // Layer
	}

	m_DefaultShader.Bind();
	m_DefaultShader.setUniformMat4("u_Transform", load_matrix);

	m_DefaultShader.setUniformMat4("u_Camera", mat4(1.0f)); // TODO

	m_DefaultShader.setUniformVec4("u_Color", m_Color);

	m_VertexArray.Bind();
	glDrawArrays(m_DrawMode, m_VerticesFirst, m_VerticesCount);
	m_VertexArray.Unbind();
	
	m_DefaultShader.Unbind();
}
