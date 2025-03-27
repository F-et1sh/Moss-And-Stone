#pragma once

namespace FE2D {
	// For Character ( Glyph )
	struct FOR_API forchar {
	public:
		// Atlas Offset of this Glyph
		vec2 atlOffset = vec2();
		// Size of Glyph
		vec2 size = vec2();
		// Offset from Baseline to Left/Top of Glyph
		vec2 bearing = vec2();
		// Horizontal Offset to Next Glyph ( Only Horizontal cuz Vertical always equals 0. Idk why )
		unsigned int advance = 0;

	public:
		forchar() = default;
		~forchar() = default;

		forchar(vec2 atlOffset, vec2 size, vec2 bearing, unsigned int advance) : atlOffset(atlOffset), size(size), bearing(bearing), advance(advance) {}
	};
}