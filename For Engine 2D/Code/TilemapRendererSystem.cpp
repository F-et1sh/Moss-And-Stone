#include "forpch.h"
#include "TilemapRendererSystem.h"

FE2D::TilemapRendererSystem::TilemapRendererSystem() {
	m_Shader.Initialize(FOR_PATH.get_shaders_path() / L"Tilemap Default" / L"TilemapDefault");
	
	m_UniformBuffer.create();
	m_UniformBuffer.bufferData((sizeof(vec4) + sizeof(mat4)) * FOR_TILE_LIMIT + sizeof(mat4) + sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
	m_UniformBuffer.bindBlock(UniformBuffer::TILEMAP_RENDERER);

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

void FE2D::TilemapRendererSystem::TilemapEditor(Entity entity, IMGUI& imgui) {
	if (!entity.HasComponent<TilemapComponent>()) return;
	auto& tilemap = entity.GetComponent<TilemapComponent>();

	ImGui::Begin("Tileset Editor");



	ImGui::End();
}

void FE2D::TilemapRendererSystem::Handle(Entity entity) {
	auto& transform = entity.GetComponent<TransformComponent>();
	auto& tilemap = entity.GetComponent<TilemapComponent>();

	for (int x = 0; x < tilemap.width; x++) {
		for (int y = 0; y < tilemap.height; y++) {
			
			uint8_t i = y * tilemap.width + x;

			if (tilemap.tiles.size() <= i || tilemap.tileset.tile_types.empty()) break;
			
			uint8_t tile = tilemap.tiles[i];
			auto& type = tilemap.tileset.tile_types[tile];

			auto& texture = m_ResourceManager->GetResource(type.texture_atlas);
			vec4 frame = type.frame;
			
			m_TextureAtlas.AddTexture(texture);
			frame += vec4(m_TextureAtlas.getTextureCoords(&texture), 0, 0);

			mat4 matrix = translate(mat4(1.0f), vec3(x * 16, y * 16, 0));
			matrix = scale(matrix, vec3(frame.z, frame.w, 0));

			m_TilesFrames.add(frame);
			m_TilesMatrices.add(matrix);

			if (m_TilesFrames.size() == FOR_TILE_LIMIT) this->Draw(entity);
		}
	}

	this->Draw(entity);
}

void FE2D::TilemapRendererSystem::Render() {
	entt::registry& registry = this->m_Scene->getRegistry();

	auto group = registry.group<TilemapComponent>(entt::get<TransformComponent>);

	for (auto e : group) {
		Entity entity{ e, m_Scene };

		this->Handle(entity);
	}
}

void FE2D::TilemapRendererSystem::Draw(Entity entity) {
	if (m_TilesFrames.empty() || m_TilesMatrices.empty()) return;

	m_Shader.Bind();
	m_TextureAtlas.bind();

	m_Shader.setUniformMat4("u_ViewProj", m_RenderContext->getViewProjection());

	constexpr size_t _atlas			 = sizeof(vec4);
	constexpr size_t _matrices		 = sizeof(mat4) * FOR_TILE_LIMIT;
	constexpr size_t _offsets		 = sizeof(vec4) * FOR_TILE_LIMIT;
	constexpr size_t _tilemap_matrix = sizeof(mat4);

	mat4 matrix = entity.GetGlobalTransform();

	m_UniformBuffer.bufferSubData(0							   , _atlas			, glm::value_ptr(m_TextureAtlas.getSize()));
	m_UniformBuffer.bufferSubData(_atlas					   , _offsets		, glm::value_ptr(m_TilesFrames[0]));
	m_UniformBuffer.bufferSubData(_atlas + _offsets			   , _matrices		, glm::value_ptr(m_TilesMatrices[0]));
	m_UniformBuffer.bufferSubData(_atlas + _offsets + _matrices, _tilemap_matrix, glm::value_ptr(matrix));

	m_VertexArray.Bind();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_TilesFrames.size());
	m_VertexArray.Unbind();

	m_TextureAtlas.unbind();
	m_Shader.Unbind();

	m_TilesFrames.reset();
	m_TilesMatrices.reset();
}