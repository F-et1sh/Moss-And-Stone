#include "forpch.h"
#include "SpriteRendererSystem.h"

void FE2D::SpriteRendererSystem::Release() {
	m_Shader.Release();
	m_UniformBuffer.release();

	m_VertexArray.Delete();
	m_VertexBuffer.Delete();

	m_TextureAtlas.Release();
}

void FE2D::SpriteRendererSystem::Initialize() {
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

void FE2D::SpriteRendererSystem::setCamera(Camera* camera) {
	m_Camera = camera;
}

void FE2D::SpriteRendererSystem::Handle(TransformComponent& transform, SpriteComponent& sprite) {
	mat4 matrix = mat4(1.0f);

	matrix = scale(matrix, vec3(
		sprite.m_FlipX ? -1 : 1,  // Flipping X
		sprite.m_FlipY ? -1 : 1,  // Flipping Y
		1));

	matrix = scale(matrix, vec3(
		sprite.m_TextureCoords.z, // Size X
		sprite.m_TextureCoords.w, // Size Y
		1));

	matrix *= (mat4)transform;

	m_Matrices.add(matrix);

	Texture* texture = m_ResourceManager->getResource<Texture>(sprite.m_TextureIndex);
	if (!texture->is_Initialized()) {
		m_AtlasOffsets.add(vec4());
		return;
	}

	m_TextureAtlas.AddTexture(*texture);

	vec4 atlas_offset = vec4(
		m_TextureAtlas.getTextureCoords(texture)				  // Texture Altas Offset
		+
		vec2(sprite.m_TextureCoords.x, sprite.m_TextureCoords.y), // Sprite Altas Offset
		vec2(sprite.m_TextureCoords.z, sprite.m_TextureCoords.w)  // Sprite Size
	);

	m_AtlasOffsets.add(atlas_offset);
}

void FE2D::SpriteRendererSystem::Update() {
	// Nothing here
}

void FE2D::SpriteRendererSystem::Render() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto view = registry.group<TransformComponent, SpriteComponent>();
	for (auto e : view) {
		auto& transform = registry.get<TransformComponent>(e);
		auto& sprite = registry.get<SpriteComponent>(e);

		this->Handle(transform, sprite);
	}

	this->DrawSprites();
}

void FE2D::SpriteRendererSystem::DrawSprites() {
	if (m_Matrices.empty() || m_AtlasOffsets.empty())
		return;

	m_Shader.Use();

	if (!m_Camera)
		m_Shader.setUniformMat4("u_Camera", glm::ortho(-1920.0f / 2, 1920.0f / 2, -1080.0f / 2, 1080.0f / 2));
	else
		m_Shader.setUniformMat4("u_Camera", *m_Camera);


	m_TextureAtlas.bind();

	constexpr size_t _atlas    = sizeof(vec4);
	constexpr size_t _matrices = sizeof(mat4) * SPRITE_LIMIT;
	constexpr size_t _offsets  = sizeof(vec4) * SPRITE_LIMIT;

	m_UniformBuffer.bufferSubData(0					, _atlas   , glm::value_ptr(m_TextureAtlas.getSize()));
	m_UniformBuffer.bufferSubData(_atlas			, _matrices, glm::value_ptr(m_Matrices[0]));
	m_UniformBuffer.bufferSubData(_atlas + _matrices, _offsets , glm::value_ptr(m_AtlasOffsets[0]));

	m_VertexArray.Bind();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_Matrices.size());
	m_VertexArray.Unbind();

	m_TextureAtlas.unbind();
	m_Shader.Unbind();

	m_Matrices.reset();
	m_AtlasOffsets.reset();
}
