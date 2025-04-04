#include "forpch.h"
#include "Texture.h"

#include "stb_image.h"

void FE2D::Texture::Release() {
	if (m_Data) 
		stbi_image_free(m_Data);

	if (m_Reference > 0)
		glDeleteTextures(1, &m_Reference);

	m_Size = vec2();
	m_Channels = 0;
	m_IsLinear = false;
}

bool FE2D::Texture::LoadFromFile(const std::wstring& file_path) {
	this->Release();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &m_Reference);
	glBindTexture(GL_TEXTURE_2D, m_Reference);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);

	// Use the STB Library to Load the Texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	const std::filesystem::path path = file_path;

	// Load file to memory
	void* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	nrChannels = STBI_rgb_alpha;

	if (!data) {
		SAY(L"ERROR : Failed to Load a Texture" <<
			L"\nPath : " << file_path <<
			L"\nSize : " << width << " " << height <<
			L"\nChannels : " << nrChannels <<
			L"\nLinear : " << m_IsLinear <<
			L"\nReason : " << stbi_failure_reason()
		);

		this->Release();

		glBindTexture(GL_TEXTURE_2D, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
		return false;
	}

	switch (nrChannels) {
		case 4: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); break;
		case 3: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB , GL_UNSIGNED_BYTE, data); break;
		case 2: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RG  , GL_UNSIGNED_BYTE, data); break;
		case 1: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_R   , GL_UNSIGNED_BYTE, data); break;
		default:
		SAY(L"ERROR : Failed to Load a Texture\nUnsupported count of channels : " << nrChannels <<
			L"\nPath : " << file_path <<
			L"\nSize : " << width << " " << height <<
			L"\nChannels : " << nrChannels <<
			L"\nLinear : " << m_IsLinear);

		this->Release();

		glBindTexture(GL_TEXTURE_2D, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
		return false;
	}
	{
		m_Size = vec2(width, height);
		m_Channels = nrChannels;

		m_Data = data;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
	
	return true;
}

void FE2D::Texture::bind() const noexcept{ glBindTexture(GL_TEXTURE_2D, m_Reference); }

inline void FE2D::Texture::setIsLinear(const bool linear) noexcept {
	m_IsLinear = linear;
	glBindTexture(GL_TEXTURE_2D, m_Reference);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

json FE2D::Texture::Serialize() const {
	json j;
	Serializable::save_value(m_IsLinear, j, "IsLinear" );
	Serializable::save_vec2	(m_Size	   , j, "Size"     );
	Serializable::save_value(m_Channels, j, "Channels" );
	return j;
}

void FE2D::Texture::Deserialize(const json& j) {
	Serializable::load_value(m_IsLinear, j, "IsLinear");
	Serializable::load_vec2 (m_Size	   , j, "Size"    );
	Serializable::load_value(m_Channels, j, "Channels");
}
