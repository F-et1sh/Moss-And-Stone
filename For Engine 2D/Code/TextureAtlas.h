#pragma once
#include "Texture.h"

namespace FE2D {
	class FOR_API TextureAtlas {
	public:
		TextureAtlas() = default;
		~TextureAtlas() { this->Release(); }
	public:
		void Release();

		void Initialize(const vec2& load_size);

		void AddTexture(Texture& load_texture);

		inline void bind()const noexcept;
		inline void unbind()const noexcept;

		// XY - Atlas Offset
		// ZW - Texture Size
		inline const vec2& getTextureCoords(Texture* texture) const {
			return m_LoadedTextures.at(texture);
		}
		inline const vec2& getSize() const noexcept { return m_AtlasSize; }

		inline const unsigned int& reference() const noexcept { return m_Reference; }

	private:
		void emplace_texture(Texture& load_texture);
		void recalculate_atlasOffset(Texture& load_texture);

	private:
		unsigned int m_Reference = 0;

	private:
		// Texture* - texture pointer to find
		// vec2 - texture coordinates in this texture atlas
		std::unordered_map<Texture*, vec2> m_LoadedTextures;

	private:
		vec2 m_AtlasSize = vec2();   // The Size of this Texture Atlas
		vec2 m_AtlasOffset = vec2(); // The Beginning of Available Space
		unsigned int maxHeight = 0;  // The Biggest Height at one Line
	};
}