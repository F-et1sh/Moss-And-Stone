#include "forpch.h"
#include "SpriteRendererSystem.h"

#include "MousePicker.h"

FE2D::SpriteRendererSystem::SpriteRendererSystem() {
	m_Shader.Initialize(FOR_PATH.get_shaders_path() / L"Sprite Default" / L"SpriteDefault");

	m_UniformBuffer.create();
	m_UniformBuffer.bufferData((sizeof(mat4) + sizeof(vec4)) * FOR_SPRITE_LIMIT + sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
	m_UniformBuffer.bindBlock(UniformBuffer::SPRITE_RENDERER);

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

	m_TextureAtlas.Initialize(FOR_TEXTURE_ATLAS_SIZE);
}

void FE2D::SpriteRendererSystem::Handle(Entity entity) {
	auto& transform = entity.GetComponent<TransformComponent>();
	auto& sprite = entity.GetComponent<SpriteComponent>();

	mat4 matrix = entity.GetGlobalTransform();

	vec2 frame_size = vec2(sprite.frame.z, sprite.frame.w);

	Texture& texture = m_ResourceManager->GetResource(sprite.texture);

	if (sprite.texture.uuid == FE2D::UUID(0))
		frame_size = texture.getSize();

	m_TextureAtlas.AddTexture(texture); // it's immediate mode. this function will just cache the texture, there is not double-adding

	matrix = scale(matrix, vec3(
		sprite.flip_x ? -1 : 1,  // flipping x
		sprite.flip_y ? -1 : 1,  // flipping y
		1));

	matrix = scale(matrix, vec3(sprite.frame.z, sprite.frame.w, 1.0f));
	m_Matrices.add(matrix);

	vec4 atlas_offset = vec4(m_TextureAtlas.getTextureCoords(&texture) + vec2(sprite.frame.x, sprite.frame.y), frame_size);

	atlas_offset = vec4(static_cast<int>(atlas_offset.x), static_cast<int>(atlas_offset.y),
						static_cast<int>(atlas_offset.z), static_cast<int>(atlas_offset.w));

	m_AtlasOffsets.add(atlas_offset);
}

void FE2D::SpriteRendererSystem::Render() {
	entt::registry& registry = this->m_Scene->getRegistry();

	size_t count = 0;

	auto group = registry.group<SpriteComponent>(entt::get<TransformComponent>);
	for (auto e : group) {
		Entity entity{ e, m_Scene };

		this->Handle(entity);

		count++;

		if (count == FOR_SPRITE_LIMIT) {
			this->DrawSprites();
			count = 0;
		}
	}

	if (count != 0)
		this->DrawSprites();
}

void FE2D::SpriteRendererSystem::RenderPickable(RenderContext& render_context, MousePicker& mouse_picker) {
	size_t count = 0;

	entt::registry& registry = this->m_Scene->getRegistry();

	auto view = registry.group<SpriteComponent>(entt::get<TransformComponent>);
	for (auto e : view) {
		Entity entity = { e, m_Scene };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& sprite = entity.GetComponent<SpriteComponent>();

		this->Handle(entity);
		m_EntityHandles.add(vec4(e, e, e, e));
		
		count++;

		if (count == FOR_SPRITE_LIMIT) {
			this->DrawPickable(render_context, mouse_picker.getShader(), mouse_picker.getUniformBuffer());
			count = 0;
		}
	}

	if (count != 0)
		this->DrawPickable(render_context, mouse_picker.getShader(), mouse_picker.getUniformBuffer());
}

void FE2D::SpriteRendererSystem::DrawPickable(RenderContext& render_context, Shader& shader, UniformBuffer& ubo) {
	if (m_Matrices.empty() || m_AtlasOffsets.empty()) return;

	shader.Bind();

	shader.setUniformMat4("u_ViewProj", m_RenderContext->getViewProjection());

	m_TextureAtlas.bind();

	constexpr size_t _atlas			 = sizeof(vec4);
	constexpr size_t _matrices		 = sizeof(mat4) * FOR_SPRITE_LIMIT;
	constexpr size_t _offsets		 = sizeof(vec4) * FOR_SPRITE_LIMIT;
	constexpr size_t _entity_handles = sizeof(vec4) * FOR_SPRITE_LIMIT;

	ubo.bufferSubData(0							   , _atlas			, glm::value_ptr(m_TextureAtlas.getSize()));
	ubo.bufferSubData(_atlas					   , _matrices		, glm::value_ptr(m_Matrices[0]));
	ubo.bufferSubData(_atlas + _matrices		   , _offsets		, glm::value_ptr(m_AtlasOffsets[0]));
	ubo.bufferSubData(_atlas + _matrices + _offsets, _entity_handles, glm::value_ptr(m_EntityHandles[0]));

	m_VertexArray.Bind();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_Matrices.size());
	m_VertexArray.Unbind();

	m_TextureAtlas.unbind();
	shader.Unbind();

	m_Matrices.reset();
	m_AtlasOffsets.reset();
	m_EntityHandles.reset();
}

void FE2D::SpriteRendererSystem::DrawSprites() {
	if (m_Matrices.empty() || m_AtlasOffsets.empty()) return;

	m_Shader.Bind();

	m_Shader.setUniformMat4("u_ViewProj", m_RenderContext->getViewProjection());

	m_TextureAtlas.bind();

	constexpr size_t _atlas    = sizeof(vec4);
	constexpr size_t _matrices = sizeof(mat4) * FOR_SPRITE_LIMIT;
	constexpr size_t _offsets  = sizeof(vec4) * FOR_SPRITE_LIMIT;

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
