#include "forpch.h"
#include "TilemapRendererSystem.h"

void FE2D::TilemapRendererSystem::UpdateTilemap(Entity entity) {
	auto& transform = entity.GetComponent<TransformComponent>();
	auto& tilemap = entity.GetComponent<TilemapComponent>();

	if (tilemap.tiles.size() < tilemap.width * tilemap.height || tilemap.tile_types.empty() || tilemap.tile_info.empty()) return;

	if (tilemap.display_tiles.size() != tilemap.tiles.size())
		tilemap.display_tiles.resize(tilemap.tiles.size());

	for (int x = 0; x < tilemap.width; x++) {
		for (int y = 0; y < tilemap.height; y++) {
			const int index = y * tilemap.width + x;

			uint8_t logic_tile = tilemap.tiles[index];
			uint8_t type = tilemap.tile_info[logic_tile].type;

			bool bad_x = x + 1 >= tilemap.width;
			bool bad_y = y + 1 >= tilemap.height;

			uint8_t id_left_bottom  = logic_tile;
			uint8_t id_right_bottom = bad_x          ? 255 : tilemap.tiles[y      * tilemap.width + (x + 1)];
			uint8_t id_right_top    = bad_x || bad_y ? 255 : tilemap.tiles[(y + 1) * tilemap.width + (x + 1)];
			uint8_t id_left_top     = bad_y          ? 255 : tilemap.tiles[(y + 1) * tilemap.width + x];

			uint8_t type_left_bottom  = tilemap.tile_info[id_left_bottom ].type;
			uint8_t type_right_bottom = bad_x          ? 255 : tilemap.tile_info[id_right_bottom].type;
			uint8_t type_right_top    = bad_x || bad_y ? 255 : tilemap.tile_info[id_right_top   ].type;
			uint8_t type_left_top     = bad_y          ? 255 : tilemap.tile_info[id_left_top    ].type;

			bool lb = tilemap.is_compatible(type, type_left_bottom);
			bool rb = tilemap.is_compatible(type, type_right_bottom);
			bool rt = tilemap.is_compatible(type, type_right_top);
			bool lt = tilemap.is_compatible(type, type_left_top);

			TilemapComponent::DisplayTile& display_tile = tilemap.display_tiles[index];
			display_tile.tile_id = logic_tile;
			display_tile.frame = TilemapComponent::get_frame(lb, rb, rt, lt);
		}
	}

	tilemap.is_updated = true;
}

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

void FE2D::TilemapRendererSystem::Handle(Entity entity) {
	auto& transform = entity.GetComponent<TransformComponent>();
	auto& tilemap = entity.GetComponent<TilemapComponent>();

	UpdateTilemap(entity);

	if (tilemap.display_tiles.size() < tilemap.width * tilemap.height || tilemap.tile_types.empty() || tilemap.tile_info.empty()) return;

	for (int x = 0; x < tilemap.width; x++) {
		for (int y = 0; y < tilemap.height; y++) {
			
			uint8_t tile = tilemap.display_tiles[y * tilemap.width + x].tile_id;
			auto& info = tilemap.tile_info[tile];

			auto& type = tilemap.tile_types[info.type];
			auto& texture = m_ResourceManager->GetResource(type.texture_atlas);
			vec4 frame = tilemap.display_tiles[y * tilemap.width + x].frame;
			
			m_TextureAtlas.AddTexture(texture);
			frame += vec4(m_TextureAtlas.getTextureCoords(&texture), 0, 0);

			m_TilesFrames.add(frame);

			mat4 matrix = translate(mat4(1.0f), vec3(x * 16, y * 16, 0));
			matrix = scale(matrix, vec3(frame.z, frame.w, 0));

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