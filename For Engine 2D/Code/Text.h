#pragma once

namespace FE2D {
	struct FOR_API Text {
	public:
		inline auto begin() { return m_String.begin(); }
		inline auto end() { return m_String.end(); }

		inline const auto begin() const { return m_String.begin(); }
		inline const auto end() const { return m_String.end(); }

		inline const wchar_t& operator[](unsigned int i) const { return m_String[i]; }

		inline operator std::wstring& () { return m_String; }
		inline operator const wchar_t* () const { return m_String.c_str(); }

		inline std::wstring operator=(std::wstring load_text) { m_String = load_text; return m_String; }

		inline bool operator==(std::wstring load_text) const { return m_String == load_text; }
		inline bool operator!=(std::wstring load_text) const { return m_String != load_text; }

	public:
		// Default Construct FOR_APIor & Destruct FOR_APIor
		Text() = default;
		~Text() = default;

		// Fast Load Text
		Text(std::wstring load_text) : m_String(load_text) {}

		// Fast Load Text and Position of this
		Text(std::wstring load_text, vec2 load_pos) : m_String(load_text), m_Position(load_pos) {}

	public:
		// Text as std::wstring
		std::wstring m_String = L"Default Text";

	public:
		// Text Position
		vec2 m_Position = vec2();

		// Text Layer
		float m_Layer = 0.0f;

		// Text Density
		vec2 m_Density = vec2(1.0f, 1.0f);

		// Text Scale
		vec2 m_Scale = vec2(1.0f, 1.0f);

		// Text Color
		vec4 m_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	public:
		enum Center {
			LEFT_BOTTOM,
			RIGHT_BOTTOM,
			MIDDLE,
			LEFT_TOP,
			RIGHT_TOP
		};
		// Flag for Center of Text
		int m_Center = MIDDLE;
	};
}