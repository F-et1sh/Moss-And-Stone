#pragma once

#include "forchar.h"

#include <fordebug.h>
#include <unordered_map>
#include <set>
#include <string>

namespace FE2D {
	class FOR_API Font {
	public:
		Font() = default;
		~Font() { this->Release(); }

		// Delete the Font Texture and Free Data of the Char Ranges
		void Release();

		// Load Symbols that will be on the Font Texture Atlas
		inline void PrepareSymbols(wchar_t load_begin, wchar_t load_end) { for (auto i = load_begin; i < load_end; i++) mLoadingChars.insert(i); }

		// Load Symbol that will be on the Font Texture Atlas
		inline void PrepareSymbol(wchar_t load_symbol) { mLoadingChars.insert(load_symbol); }

		// Load all of Unique Symbols from Text
		inline void PrepareText(std::wstring& text_data) {
			std::set<wchar_t> uniqueChars;
			for (const auto& ch : text_data) { uniqueChars.insert(ch); }
			for (auto i = uniqueChars.begin(); i != uniqueChars.end(); i++) mLoadingChars.insert(static_cast<int>(*i));
		}

		// Finish Setting and Create the Texture Atlas
		bool BuildFont(const char* path, vec2 load_atlasSize, vec2 load_pixelSize, bool isLinear);

	private:
		// Load Symbols and Add they to the Char List
		unsigned int LoadSymbols(std::set<wchar_t>& loading_charList, vec2& pixelSize);

	public:
		// Bind the Font Texture Atlas as
		inline void bind_as(unsigned int load_texQueue_num)const;

		// Get Size of the Font Texture Atlas
		inline const vec2& getAtlasSize() const { return mAtlasSize; }

		// Get Pixel Size of Glyphs
		inline const vec2& getPixelSize() const { return mPixelSize; }

#ifdef _DEBUG
		// Get a Char from the List
		inline const forchar& getChar(wchar_t index) const {
			auto it = std::find_if(mCharList.begin(), mCharList.end(), [&index](const auto& pair) { return pair.first == index; });
			if (it != mCharList.end()) return it->second;
			else {
				SAY(L"ERROR : Unknown Letter : " << index << L"\nFont Texture : " << mFontTex << L"\nAtlas Size : " << mAtlasSize.x << " " << mAtlasSize.y << L"\nPixel Size : " << mPixelSize.x << L" " << mPixelSize.y);
				return forchar();
			}
		}
#else
		// Get a Char from the List
		inline const forchar& getChar(wchar_t index) { return mCharList[index]; }
#endif

		// Get a Reference to the Texture Atlas of this Font
		inline const unsigned int& reference() const { return mFontTex; }

	private:
		// List of Characters on the Font Map
		std::unordered_map<wchar_t, forchar> mCharList;

	private:
		// Texture of the Font Texture Atlas
		unsigned int mFontTex = 0;

	private:
		// Size of Font Texture Atlas
		vec2 mAtlasSize = vec2();

		// Begin of Last Available Space
		vec2 mAtlasOffset = vec2();

		// Pixel Size of Glyhs
		vec2 mPixelSize = vec2();

		// Biggest Height at one Line ( \n )
		unsigned int maxHeight = 0;

	private:
		// Chars to Load
		std::set<wchar_t> mLoadingChars;
	};
}