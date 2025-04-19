#pragma once
#include "TextureAtlas.h"
#include "Window.h"

namespace FE2D {
	class FOR_API Test {
	public:
		Test() = default;
		~Test() = default;

		void init();
		void loop();
	private:
		Window window;

		Camera camera;

		Shader shader;
		UniformBuffer ubo;

		VertexArray vao;
		VertexBuffer vbo;
	private:
		TextureAtlas atlas;
		Texture texture;

		inline static constexpr size_t matrices_size = 256;
		FE2D::dynamic_array<mat4, matrices_size> matrices;
		FE2D::dynamic_array<vec4, matrices_size> texture_offsets;

		vec2 cursor_pos = vec2();
	};
}