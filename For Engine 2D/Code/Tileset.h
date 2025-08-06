#pragma once

#undef max

namespace FE2D {
	using TileID = uint64_t;

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

		bool LoadFromFile(const std::filesystem::path& file_path)override;
		void UploadToFile(const std::filesystem::path& file_path)const override;

		inline bool hasTile(TileID index)const noexcept { return index < m_Tiles.size(); }

		inline Tile& addTile(const Tile& tile) { return m_Tiles.emplace_back(tile); }

		inline Tile& getTile(TileID index) {
			if (!hasTile(index)) FOR_RUNTIME_ERROR("Failed to get tile by lvalue. Use hasTile() to check\nIndex : " + std::to_string(index) + "\nTileset size : " + std::to_string(m_Tiles.size()));
			return m_Tiles[index];
		}

		inline Tile getTile(TileID index)const noexcept {
			if (!hasTile(index)) return {};
			return m_Tiles[index];
		}

		// if your index is greater than the array size, it will be incremented
		inline Tile& setTile(TileID index, const Tile& tile) {
			if (m_Tiles.size() >= index) m_Tiles.resize(index);
			return m_Tiles[index] = tile;
		}

		inline void removeTile(TileID index)noexcept {
			if (index >= m_Tiles.size()) return;
			m_Tiles.erase(m_Tiles.begin() + index);
		}

	private:
		std::vector<Tile> m_Tiles;
	};
}
