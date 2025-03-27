#pragma once
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Camera.h"

#define FOR_MESH_2D_SHADER_VERT L"Files/Shaders/Default/Mesh2D/Mesh2D.vert"
#define FOR_MESH_2D_SHADER_FRAG L"Files/Shaders/Default/Mesh2D/Mesh2D.frag"

namespace FE2D {
	class FOR_API Mesh2D {
	public:
		Mesh2D() = default;
		~Mesh2D() = default;

		void Release();
		void Initialize(const vec2* vertices, const unsigned int vertices_first, const unsigned int vertices_count, const int usage, const unsigned int draw_mode);

		void UpdateMesh(const vec2* vertices, const unsigned int offset, const unsigned int size);

		inline const dynamic_array<vec2>& getVertices()const noexcept { return m_VertexData; };

		inline void draw()const;

	public:
		vec2 m_Position = vec2();

		float m_Layer = 0.0f;

		// Flag for Auto Sprite Sorting
		bool m_AutoSort = false;

		vec2 m_Size = vec2(1.0f, 1.0f);

		float m_Rotation = 0.0f;
		vec2  m_Origin = vec2();

		vec4 m_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	private:
		dynamic_array<vec2> m_VertexData;
	private:
		Shader m_DefaultShader;

		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;

		unsigned int m_VerticesFirst = 0;
		unsigned int m_VerticesCount = 0;

		unsigned int m_DrawMode = 0;
	};
}