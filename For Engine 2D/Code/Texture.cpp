#include "forpch.h"
#include "Texture.h"

#include "stb_image.h"

FE2D::Texture::~Texture() {
	if (m_Data) stbi_image_free(m_Data);
	if (m_Reference > 0) glDeleteTextures(1, &m_Reference);
}

bool FE2D::Texture::LoadFromFile(const std::filesystem::path& file_path) {
	this->Release();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_Reference);
	glBindTexture(GL_TEXTURE_2D, m_Reference);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);

	// Use the STB Library to Load the Texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	// Load file to memory
	void* data = stbi_load(file_path.string().c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
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

void FE2D::Texture::OnEditorDraw(IMGUI& imgui) {
	imgui.CheckBox("Is Linear", m_IsLinear);

	setIsLinear(m_IsLinear);
}

void FE2D::Texture::Bind() const noexcept { glBindTexture(GL_TEXTURE_2D, m_Reference); }
void FE2D::Texture::Unbind() const noexcept{ glBindTexture(GL_TEXTURE_2D, 0); }

inline void FE2D::Texture::setIsLinear(bool linear) noexcept {
	m_IsLinear = linear;
	glBindTexture(GL_TEXTURE_2D, m_Reference);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_IsLinear ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

json FE2D::Texture::Serialize() const {
	json j;
	SceneSerializer::save_value(m_IsLinear, j, "IsLinear");
	return j;
}

void FE2D::Texture::Deserialize(const json& j) {
	SceneSerializer::load_value(m_IsLinear, j, "IsLinear");
	
	setIsLinear(m_IsLinear);
}
