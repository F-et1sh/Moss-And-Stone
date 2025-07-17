#pragma once

#undef max

namespace FE2D {
	using TileID = uint8_t;
	inline constexpr static size_t FOR_TILES_COUNT = std::numeric_limits<TileID>::max() + 1;

	struct FOR_API Tile {
		ivec4 uv = ivec4();
		ResourceID<Texture> texture_id;

		Tile() = default;
		~Tile() = default;
		
		Tile(const ivec4& uv, ResourceID<Texture> texture_id) : uv(uv), texture_id(texture_id) {}
	};

	class FOR_API Tileset : public IResource {
	public:
		Tileset() = default;
		~Tileset() = default;

		bool LoadFromFile(const std::filesystem::path& file_path) override;
		void UploadToFile(const std::filesystem::path& file_path) const override;

		inline const Tile& getTile(TileID index)const noexcept { return m_Tiles[index]; /* can't throw an error */ }
		inline const Tile& setTile(TileID index, const Tile& tile)noexcept { return m_Tiles[index] = tile; }

		inline const Tile& addTile(const Tile& tile) {
			size_t i = m_TilesCount;
			if (m_TilesCount++ >= FOR_TILES_COUNT)
				FOR_RUNTIME_ERROR("Failed to add tile. Tileset overflow\nMax count : " + std::to_string(FOR_TILES_COUNT));
			return m_Tiles[i] = tile;
		}

	private:
		std::array<Tile, FOR_TILES_COUNT> m_Tiles;
		size_t m_TilesCount = 0;
	};
}
