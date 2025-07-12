#include "forpch.h"
#include "TextureAtlas.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

FE2D::TextureAtlas::~TextureAtlas() {
	glDeleteTextures(1, &m_Reference);
}

void FE2D::TextureAtlas::Initialize(const vec2& load_size) {
	glGenTextures(1, &m_Reference);
	glBindTexture(GL_TEXTURE_2D, m_Reference);

	// Texture Setting
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Create the Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, load_size.x, load_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// Set the Atlas Size
	m_AtlasSize = load_size;

	// Unbind the Texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FE2D::TextureAtlas::AddTexture(Texture& load_texture) {
	FOR_ASSERT(m_Reference, "Failed to Add a Texture to Texture Atlas\nTexture Atlas is not Initialized");
	FOR_ASSERT(load_texture.is_Initialized(), "Failed to Add a Texture to Texture Atlas\nTexture is not Initialized");

	const auto it = m_LoadedTextures.find(&load_texture);
	if (it != m_LoadedTextures.end()) return;

	emplace_texture(load_texture); // add the texture to the opengl texture

	m_LoadedTextures.emplace(&load_texture, m_AtlasOffset);

	recalculate_atlas_offset(load_texture);
}

inline void TextureAtlas::bind() const noexcept { glBindTexture(GL_TEXTURE_2D, m_Reference); }

inline void FE2D::TextureAtlas::unbind() const noexcept { glBindTexture(GL_TEXTURE_2D, 0); }

void FE2D::TextureAtlas::emplace_texture(Texture& load_texture) {
	glBindTexture(GL_TEXTURE_2D, m_Reference);

	if (!load_texture.getData()) {
		SAY("ERROR : Failed to emplate a texture in TextureAtlas\nTexture data was nullptr");
		return;
	}

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		m_AtlasOffset.x,
		m_AtlasOffset.y,
		load_texture.getSize().x,
		load_texture.getSize().y,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		load_texture.getData()
	);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FE2D::TextureAtlas::recalculate_atlas_offset(Texture& load_texture) {
	const unsigned int width = load_texture.getSize().x;
	const unsigned int height = load_texture.getSize().y;

	constexpr int padding = 1;

	if (m_AtlasOffset.x + width + padding > m_AtlasSize.x) {
		m_AtlasOffset.x = 0;
		m_AtlasOffset.y += m_MaxHeight + padding;
		m_MaxHeight = 0;
	}

	if (m_AtlasOffset.y + height + padding > m_AtlasSize.y) {
		FOR_RUNTIME_ERROR(
			"ERROR : Texture Atlas is Overfull\nTexture Atlas Size : " +
			std::to_string(m_AtlasSize.x) + " x " + std::to_string(m_AtlasSize.y) +
			"\nFailed to fit Texture : " + std::to_string(width) + " x " + std::to_string(height) +
			"\nPadding : " + std::to_string(padding));
	}

	if (height > m_MaxHeight)
		m_MaxHeight = height;

	m_AtlasOffset.x += width + padding;
}