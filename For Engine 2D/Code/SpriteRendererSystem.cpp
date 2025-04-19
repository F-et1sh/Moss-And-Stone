#include "forpch.h"
#include "SpriteRendererSystem.h"

#include "RenderContext.h"
#include "MousePicker.h"

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
	m_UniformBuffer.bindBlock(1);

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

void FE2D::SpriteRendererSystem::Handle(TransformComponent& transform, SpriteComponent& sprite) {

	mat4 matrix = transform;
	
	matrix = scale(matrix, vec3(
		sprite.flip_x ? -1 : 1,  // Flipping X
		sprite.flip_y ? -1 : 1,  // Flipping Y
		1));

	matrix = scale(matrix, vec3(
		sprite.texture_coords.z, // Size X
		sprite.texture_coords.w, // Size Y
		1));

	m_Matrices.add(matrix);

	Texture& texture = m_ResourceManager->getResource<Texture>(sprite.texture_index);

	m_TextureAtlas.AddTexture(texture);

	vec4 atlas_offset = vec4(
		m_TextureAtlas.getTextureCoords(&texture)				// Texture Altas Offset
		+
		vec2(sprite.texture_coords.x, sprite.texture_coords.y), // Sprite Altas Offset
		vec2(sprite.texture_coords.z, sprite.texture_coords.w)  // Sprite Size
	);

	m_AtlasOffsets.add(atlas_offset);
}

void FE2D::SpriteRendererSystem::Update() {

}

void FE2D::SpriteRendererSystem::Render() {
	entt::registry& registry = this->m_Scene->getRegistry();

	size_t count = 0;

	auto view = registry.group<TransformComponent, SpriteComponent>();
	for (auto e : view) {
		auto& transform = registry.get<TransformComponent>(e);
		auto& sprite = registry.get<SpriteComponent>(e);

		this->Handle(transform, sprite);

		count++;

		if (count == SPRITE_LIMIT) {
			this->DrawSprites();
			count = 0;
		}
	}

	if (count != 0)
		this->DrawSprites();
}

void FE2D::SpriteRendererSystem::RenderPickable(RenderContext& render_context, MousePicker& mouse_picker) {
	m_EntityHandles.set_capacity(SPRITE_LIMIT);

	size_t count = 0;

	entt::registry& registry = this->m_Scene->getRegistry();

	auto view = registry.group<TransformComponent, SpriteComponent>();
	for (auto e : view) {
		auto& transform = registry.get<TransformComponent>(e);
		auto& sprite = registry.get<SpriteComponent>(e);

		mat4 matrix = transform;

		matrix = scale(matrix, vec3(
			sprite.flip_x ? -1 : 1,  // Flipping X
			sprite.flip_y ? -1 : 1,  // Flipping Y
			1));

		matrix = scale(matrix, vec3(
			sprite.texture_coords.z, // Size X
			sprite.texture_coords.w, // Size Y
			1));

		m_EntityHandles.add(vec4(e, e, e, e));
		m_Matrices.add(matrix);

		count++;

		if (count == SPRITE_LIMIT) {
			this->DrawPickable(render_context, mouse_picker.getShader(), mouse_picker.getUniformBuffer());
			count = 0;
		}
	}

	if (count != 0)
		this->DrawPickable(render_context, mouse_picker.getShader(), mouse_picker.getUniformBuffer());
}

void FE2D::SpriteRendererSystem::DrawPickable(RenderContext& render_context, Shader& shader, UniformBuffer& ubo) {
	if (m_Matrices.empty())
		return;

	shader.Use();

	shader.setUniformMat4("u_ViewProj", render_context.getViewProjection());

	constexpr size_t _matrices = sizeof(mat4) * SPRITE_LIMIT;
	constexpr size_t _entity_handles = sizeof(vec4) * SPRITE_LIMIT;

	ubo.bufferSubData(0		   , _matrices		, m_Matrices	 .reference());
	ubo.bufferSubData(_matrices, _entity_handles, m_EntityHandles.reference());

	m_VertexArray.Bind();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_Matrices.size());
	m_VertexArray.Unbind();

	shader.Unbind();

	m_Matrices.reset();
	m_EntityHandles.reset();
}

void FE2D::SpriteRendererSystem::DrawSprites() {
	if (m_Matrices.empty() || m_AtlasOffsets.empty())
		return;

	m_Shader.Use();

	m_Shader.setUniformMat4("u_ViewProj", m_RenderContext->getViewProjection());

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
