#include "forpch.h"
#include "Test.h"

void FE2D::Test::init() {
	GLFW::Initialize();

	window.Initialize(vec2(800, 600), "Test", -1);

	shader.Initialize(L"C:/Users/home/Desktop/Moss And Stone/Files/Shaders/Test Default/TestDefault");
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

	texture.LoadFromFile(L"C:\\Users\\home\\Desktop\\Moss And Stone\\Files\\Resources\\5260531646027920540.jpg");
	
	atlas.Initialize(vec2(4096, 4096));
	atlas.AddTexture(texture);

	camera.BindToWindow(&this->window);
}

static double a = 0.0;

void FE2D::Test::loop() {
	while (window.isOpen()) {
		window.Update_DeltaTime();
		a += window.getDeltaTime();

		window.ClearColor(vec4(0.12f, 0.12f, 0.12f, 1.0f));
		window.ClearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (size_t i = 0; i < matrices_size; i++) {
			const glm::mat4 matrix = glm::scale(glm::mat4(1.0f), glm::vec3(512.0f)) * glm::rotate(glm::mat4(1.0f), (float)(a + i), glm::vec3(0, 0, 1));
			matrices.add(matrix);
			texture_offsets.add(vec4(0, 0, texture.getSize()));
		}

		shader.Use();

		atlas.bind();

		ubo.bufferSubData(0                  , 16              , &atlas.getSize());
		ubo.bufferSubData(16                 , 64*matrices_size, &matrices[0]);
		ubo.bufferSubData(16+64*matrices_size, 16*matrices_size, &texture_offsets[0]);

		shader.setUniformMat4("u_Camera", camera);

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
