#pragma once

namespace FE2D {
	static constexpr size_t FOR_TILE_LIMIT = 256; // limit of tiles per one drawcall
}

namespace FE2D {
	class FOR_API TilemapRendererSystem : public ISystemBase {
	public:
		TilemapRendererSystem();
		~TilemapRendererSystem() = default;

		void Render();

	private:
		void Draw(Texture& texture_atlas);

	private:
		Shader m_Shader;
		UniformBuffer m_UniformBuffer;

		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;

		// xy - position
		// zw - atlas offset
		FE2D::dynamic_array<vec4, FOR_TILE_LIMIT> m_TileInfo;
	};
}