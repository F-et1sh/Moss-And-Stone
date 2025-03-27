#include "forpch.h"
#include "Font.h"

// FreeType Library Initializing
static FT_Library ft;

// FreeType Face - thing for Functions of FreeType Library
static FT_Face face;

// Delete the Font Texture and Free Data of the Char Ranges
void Font::Release() {
	if (mFontTex) glDeleteTextures(1, &mFontTex);
	mCharList.clear();
}

// Load Symbols and Add they to the Char List
unsigned int Font::LoadSymbols(std::set<wchar_t>& loading_charList, vec2& pixelSize) {
	// Disable Byte-Alignment Restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Bind the Font Texture
	glBindTexture(GL_TEXTURE_2D, mFontTex);

	unsigned int counter = 0;

	// Load from Begin to End Simbols of ASCII Set
	for (auto i = loading_charList.begin(); i != loading_charList.end(); i++) {
		// Load Character's Glyph
		if (FT_Load_Char(face, *i, FT_LOAD_RENDER) != 0) SAY("ERROR : Failed to Load Char\nChar ID : " << int(*i));
		else {
			// Set the Settings of Current Character
			mCharList.insert(std::pair<wchar_t, forchar>(*i, forchar(
				vec2(mAtlasOffset.x, mAtlasOffset.y),
				vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x >> 6
			)));

			// Add the Char to the Texture Atlas
			glTexSubImage2D(
				GL_TEXTURE_2D,
				NULL,
				static_cast<int>(mAtlasOffset.x),
				static_cast<int>(mAtlasOffset.y),
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			unsigned int width = face->glyph->bitmap.width;
			unsigned int height = face->glyph->bitmap.rows;

			float offset = pixelSize.x;

			if (maxHeight < height) maxHeight = height;
			if (mAtlasOffset.x + width + offset <= mAtlasSize.x) mAtlasOffset.x += width;
			else {
				mAtlasOffset.x = 0;
				if (mAtlasOffset.x + width + offset <= mAtlasSize.x &&
					mAtlasOffset.y + height + offset <= mAtlasSize.y) {
					mAtlasOffset.y += static_cast<float>(maxHeight);
					maxHeight = 0;
				}
				else {
					SAY("ERROR : Failed to Load Symbols to the Font");
					return counter;
				}
			}
			counter++;
		}
	}

	// Enable Byte-Alignment Restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, NULL);

	return counter;
}

// Finish Setting and Create the Texture Atlas
bool Font::BuildFont(const char* path, vec2 load_atlasSize, vec2 load_pixelSize, bool isLinear) {
	// All Functions will Return a Value which Different than NULL whenever an Error Occurred
	if (FT_Init_FreeType(&ft) != NULL) { SAY("ERROR : Failed to Initialize FreeType Library"); return false; }

	// Load the Font as the Face
	if (FT_New_Face(ft, path, NULL, &face) != NULL) { SAY("ERROR : Failed to Load Font\nPath : " << path); return false; }
	else {
		// Set the Pixel Size
		mPixelSize = load_pixelSize;
		FT_Set_Pixel_Sizes(face, load_pixelSize.x, load_pixelSize.y);

		// Texture Setting
		{
			// If the Font Texture already Create Delete it
			if (mFontTex) glDeleteTextures(1, &mFontTex);

			// Create the Texture
			glCreateTextures(GL_TEXTURE_2D, 1, &mFontTex);
			// Bind the Texture
			glBindTexture(GL_TEXTURE_2D, mFontTex);

			// Set the Atlas Size
			mAtlasSize = load_atlasSize;

			// Generate the Texture
			glTexImage2D(GL_TEXTURE_2D, NULL, GL_RED, load_atlasSize.x, load_atlasSize.y, NULL, GL_RED, GL_UNSIGNED_BYTE, nullptr);

			// Set the Texture Options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			if (isLinear) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		}

		// Load all of Lists of Chars
		auto result = LoadSymbols(mLoadingChars, load_pixelSize);
		if (result != mLoadingChars.size()) {
			SAY("ERROR : Failed to Load Symbols\nCount of Loading Symbols : " << mLoadingChars.size() << "\nLast Loaded : " << result);

			mLoadingChars.clear();

			glBindTexture(GL_TEXTURE_2D, 0);

			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			return false;
		}

		// Delete all of Lists of Chats after Loading
		mLoadingChars.clear();

		// Unbind the Texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Destroy FreeType Library
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return true;
}

// Bind the Font Texture Atlas as
inline void Font::bind_as(unsigned int load_texQueue_num)const { glActiveTexture(GL_TEXTURE0 + load_texQueue_num); glBindTexture(GL_TEXTURE_2D, mFontTex); }