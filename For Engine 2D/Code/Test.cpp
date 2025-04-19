#include "forpch.h"
#include "Test.h"

void FE2D::Test::init() {
	GLFW::Initialize();

	window.Initialize(vec2(800, 600), "Test", -1);

	shader.Initialize(FOR_PATH.get_shaders_path() / L"Test Default/TestDefault");
	ubo.create();
	ubo.bufferData((16 + 64)*matrices_size+16, nullptr, GL_DYNAMIC_DRAW);
	ubo.bindBlock(0);

	vao.Create();
	vao.Bind();

	vbo.Create();
	vbo.Bind();

	// Default Rectangle Vertices
	float load_vertices[]{
		0.0f - 0.5f, 1.0f - 0.5f,
		0.0f - 0.5f, 0.0f - 0.5f,
		1.0f - 0.5f, 1.0f - 0.5f,
		1.0f - 0.5f, 0.0f - 0.5f,
	};

	// Pass the Square Canvas just One Time
	vbo.BufferData(sizeof(load_vertices), load_vertices, GL_STATIC_DRAW);

	// Enable and Set the Vertex Attrib Pointer
	vao.EnableAttrib(0);
	vao.AttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (const void*)0);

	vbo.Unbind();
	vao.Unbind();

	texture.LoadFromFile(FOR_PATH.get_resources_path() / L"17391194830148395414519475130122.jpg");
	
	atlas.Initialize(vec2(4096, 4096*3));
	atlas.AddTexture(texture);
}

static double a = 0.0;

void FE2D::Test::loop() {
	while (window.isOpen()) {
		window.Update_DeltaTime();
		a += window.getDeltaTime();

		window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (size_t i = 0; i < matrices_size; i++) {
			mat4 matrix = mat4(1.0f);
			matrix = translate(matrix, vec3());
			matrix = rotate(matrix, radians(float(a+i))*15, vec3(0, 0, 1));
			matrix = scale(matrix, vec3(texture.getSize(), 0) / vec3(4));

			matrices.add(matrix);
			texture_offsets.add(vec4(0, 0, texture.getSize()));
		}

		shader.Use();

		atlas.bind();

		ubo.bufferSubData(0                  , 16              , &atlas.getSize());
		ubo.bufferSubData(16                 , 64*matrices_size, &matrices[0]);
		ubo.bufferSubData(16+64*matrices_size, 16*matrices_size, &texture_offsets[0]);

		shader.setUniformMat4("u_Camera", mat4(1.0f)); // stub

		vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, matrices_size);
		vao.Unbind();

		matrices.reset();
		texture_offsets.reset();
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
		shader.Unbind();

		window.SwapBuffers();
		window.PollEvent();
	}
}
