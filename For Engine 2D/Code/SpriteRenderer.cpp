#include "forpch.h"
#include "SpriteRenderer.h"

FOR_RENDERER_REGISTER(SpriteRenderer, USpriteComponent, "SpriteRenderer");

void FE2D::SpriteRenderer::Release() {
	m_Shader.Release();
	m_UniformBuffer.release();

	m_VertexArray.Delete();
	m_VertexBuffer.Delete();

	m_TextureAtlas.Release();
}

void FE2D::SpriteRenderer::Initialize(ResourceManager* resourceManager) {
	if (!resourceManager)
		FOR_RUNTIME_ERROR("Failed to Initialize SpriteRenderer\nResourceManager was nullptr");
	m_ResourceManager = resourceManager;

	m_Shader.Initialize(FOR_PATH.get_shaders_path() / L"Sprite Default" / L"SpriteDefault");

	m_UniformBuffer.create();
	m_UniformBuffer.bufferData((sizeof(mat4) + sizeof(vec4)) * SPRITE_LIMIT + sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
	m_UniformBuffer.bindBlock(0);

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

	m_TextureAtlas.Initialize(TEXTURE_ATLAS_SIZE);
}

void FE2D::SpriteRenderer::setCamera(Camera* camera) {
	m_Camera = camera;
}

void FE2D::SpriteRenderer::Handle(UTransformComponent* transform, const UComponent* component) {
	const USpriteComponent* sprite = static_cast<const USpriteComponent*>(component);

	mat4 matrix = scale(vec3(
		sprite->m_FlipX ? -1 : 1,  // Flipping X
		sprite->m_FlipY ? -1 : 1,  // Flipping Y
		1));
	matrix *= scale(vec3(
		sprite->m_TextureCoords.z, // Size X
		sprite->m_TextureCoords.w, // Size Y
		1));
	matrix *= transform->getTransform();

	m_Matrices.add(matrix);

	Texture* texture = m_ResourceManager->getResource<Texture>(sprite->m_TextureIndex);
	if (!texture->is_Initialized()) {
		m_AtlasOffsets.add(vec4());
		return;
	}

	m_TextureAtlas.AddTexture(*texture);

	vec4 atlas_offset = vec4(
		m_TextureAtlas.getTextureCoords(texture)					// Texture Altas Offset
		+
		vec2(sprite->m_TextureCoords.x, sprite->m_TextureCoords.y), // Sprite  Altas Offset
		vec2(sprite->m_TextureCoords.z, sprite->m_TextureCoords.w)  // Sprite Size
	);

	m_AtlasOffsets.add(atlas_offset);
}

void FE2D::SpriteRenderer::Render() {
	if (m_Matrices.empty() || m_AtlasOffsets.empty())
		return;
	
	m_Shader.Use();

	if (!m_Camera)
		m_Shader.setUniformMat4("u_Camera", mat4(1.0f));
	else
		m_Shader.setUniformMat4("u_Camera", *m_Camera);


	m_TextureAtlas.bind();

	const size_t _atlas    = sizeof(vec4);
	const size_t _matrices = sizeof(mat4) * SPRITE_LIMIT;
	const size_t _offsets  = sizeof(vec4) * SPRITE_LIMIT;

	m_UniformBuffer.bufferSubData(0				  , _atlas	 , &m_TextureAtlas.getSize());
	m_UniformBuffer.bufferSubData(_atlas		  , _matrices, &m_Matrices[0]);
	m_UniformBuffer.bufferSubData(_atlas+_matrices, _offsets , &m_AtlasOffsets[0]);
	
	m_VertexArray.Bind();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_Matrices.size());
	m_VertexArray.Unbind();

	m_TextureAtlas.unbind();
	m_Shader.Unbind();

	m_Matrices.reset();
	m_AtlasOffsets.reset();
	
}