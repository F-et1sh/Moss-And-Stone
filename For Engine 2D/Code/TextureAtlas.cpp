#include "forpch.h"
#include "TextureAtlas.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void FE2D::TextureAtlas::Release() {
	if (m_Reference) glDeleteTextures(1, &m_Reference);
	m_LoadedTextures.clear();
}

void FE2D::TextureAtlas::Initialize(const vec2& load_size) {
	this->Release();

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
	if (!m_Reference) {
		SAY("ERROR : Failed to Add a Texture to Texture Atlas\nTexture Atlas is not Initialized");
		return;
	}
	if (!load_texture.is_Initialized()) {
		SAY("ERROR : Failed to Add a Texture to Texture Atlas\nTexture is not Initialized");
		return;
	}
	const auto it = m_LoadedTextures.find(&load_texture);

	if (it != m_LoadedTextures.end())
		return;

	emplace_texture(load_texture); // add the texture to the opengl texture

	m_LoadedTextures.emplace(&load_texture, m_AtlasOffset);

	recalculate_atlasOffset(load_texture);
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

void FE2D::TextureAtlas::recalculate_atlasOffset(Texture& load_texture) {
	unsigned int width = load_texture.getSize().x;
	unsigned int height = load_texture.getSize().y;
	unsigned int offset = 0; // doesn't using for now

	if (maxHeight < height) maxHeight = height;
	if (m_AtlasOffset.x + width + offset <= m_AtlasSize.x) m_AtlasOffset.x += width;
	else {
		m_AtlasOffset.x = 0;
		if (m_AtlasOffset.x + width + offset <= m_AtlasSize.x &&
			m_AtlasOffset.y + height + offset <= m_AtlasSize.y) {
			m_AtlasOffset.y += maxHeight;
			maxHeight = 0;
		}
		else
			FOR_RUNTIME_ERROR("ERROR : Texture Atlas is Overfull\nTexture Atlas Size : " +
				std::to_string(m_AtlasSize.x) + " " + std::to_string(m_AtlasSize.y));
	}
}
