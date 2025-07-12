#pragma once
#include "Texture.h"

namespace FE2D {
	class FOR_API TextureAtlas {
	public:
		TextureAtlas() = default;
		~TextureAtlas();
		
		TextureAtlas(const vec2& load_size) { this->Initialize(load_size); }

		// legacy
		void Initialize(const vec2& load_size);

		void AddTexture(Texture& load_texture);

		inline void bind()const noexcept;
		inline void unbind()const noexcept;

		// XY - Atlas Offset
		// ZW - Texture Size
		inline vec2 getTextureCoords(Texture* texture) const {
			const auto it = m_LoadedTextures.find(texture);
			FOR_ASSERT(it != m_LoadedTextures.end(), "Texture not found in atlas");
			return it->second;
		}
		inline vec2 getSize() const noexcept { return m_AtlasSize; }
		inline unsigned int reference() const noexcept { return m_Reference; }

	private:
		void emplace_texture(Texture& load_texture);
		void recalculate_atlas_offset(Texture& load_texture);

		unsigned int m_Reference = 0;

		// Texture* - texture pointer to find
		// vec2 - texture coordinates in this texture atlas
		std::unordered_map<Texture*, vec2> m_LoadedTextures;

		vec2		 m_AtlasSize   = vec2(); // The Size of this Texture Atlas
		vec2		 m_AtlasOffset = vec2(); // The Beginning of Available Space
		unsigned int m_MaxHeight   = 0;		 // The Biggest Height at one Line
	};
}