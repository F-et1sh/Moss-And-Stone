#pragma once

namespace FE2D {
	static constexpr size_t FOR_TILE_LIMIT = 256; // limit of tiles per one drawcall
}

namespace FE2D {
	class FOR_API TilemapRendererSystem : public ISystemBase {
	public:
		TilemapRendererSystem();
		~TilemapRendererSystem() = default;

		// this function will update the display tiles of tilemap even it's already updated
		static void UpdateTilemap(Entity entity);

		void Render();

	private:
		void Handle(Entity entity);
		void Draw(Entity entity);

	private:
		Shader m_Shader;
		UniformBuffer m_UniformBuffer;

		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;

		TextureAtlas m_TextureAtlas;

		FE2D::dynamic_array<vec4, FOR_TILE_LIMIT> m_TilesFrames;
		FE2D::dynamic_array<mat4, FOR_TILE_LIMIT> m_TilesMatrices;
	};
}