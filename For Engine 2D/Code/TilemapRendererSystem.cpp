#include "forpch.h"
#include "TilemapRendererSystem.h"

FE2D::TilemapRendererSystem::TilemapRendererSystem() {
	m_Shader.Initialize(FOR_PATH.get_shaders_path() / L"Tilemap Default" / L"TilemapDefault");
	
	m_UniformBuffer.create();
	m_UniformBuffer.bufferData(sizeof(vec4) * FOR_TILE_LIMIT + sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);
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
}

void FE2D::TilemapRendererSystem::Render() {
	entt::registry& registry = this->m_Scene->getRegistry();
	
	auto group = registry.group<TilemapComponent>(entt::get<TransformComponent>);
	for (auto e : group) {
		Entity entity{ e, m_Scene };
		auto& tilemap = entity.GetComponent<TilemapComponent>();

		//if (tilemap.tiles.size() < tilemap.width * tilemap.height)
		//	continue;

		//for (int x = 0; x < tilemap.width; x++) {
		//	for (int y = 0; y < tilemap.height; y++) {
		//		uint8_t texture_index = tilemap.tiles[y * tilemap.width + x];
		//		auto texture_id = tilemap.textures[texture_index];

		//		/*vec2 atlas_offset = vec2(0, 0);
		//		if (texture_id.uuid != FE2D::UUID(0)) {
		//			auto& texture = m_ResourceManager->GetResource(texture_id);
		//			tilemap.texture_atlas.AddTexture(texture);

		//			atlas_offset = tilemap.texture_atlas.getTextureCoords(&texture);
		//		}
		//		
		//		m_TileInfo.add({ vec2(x, y), atlas_offset });*/
		//	}
		//}

		//this->Draw(tilemap.texture_atlas);
	}
}

void FE2D::TilemapRendererSystem::Draw(Texture& texture_atlas) {
	/*if (m_TileInfo.empty()) return;

	m_Shader.Bind();

	m_Shader.setUniformMat4("u_ViewProj", m_RenderContext->getViewProjection());

	texture_atlas.Bind();

	constexpr size_t _tile_info = sizeof(vec4) * FOR_TILE_LIMIT;
	
	m_UniformBuffer.bufferSubData(0, _tile_info, m_TileInfo.reference());
	m_UniformBuffer.bufferSubData(0, _tile_info, m_TileInfo.reference());

	m_VertexArray.Bind();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_Matrices.size());
	m_VertexArray.Unbind();

	texture_atlas.Unbind();
	m_Shader.Unbind();

	m_Matrices.reset();
	m_AtlasOffsets.reset();*/
}