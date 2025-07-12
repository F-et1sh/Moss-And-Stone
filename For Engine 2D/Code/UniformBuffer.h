#pragma once

namespace FE2D {
	class FOR_API UniformBuffer {
	public:
		inline static constexpr unsigned int MOUSE_PICKER = 0;
		inline static constexpr unsigned int SPRITE_RENDERER = 1;
		inline static constexpr unsigned int TILEMAP_RENDERER = 2;

	public:
		UniformBuffer() = default;
		~UniformBuffer() { this->release(); }

		inline void release()noexcept;
		inline void create()noexcept;

		inline void bindBlock(unsigned int block_index)noexcept;

		inline void bufferData(unsigned int load_size, const void* load_data, int update_mode)noexcept;
		inline void bufferSubData(unsigned int load_offset, unsigned int load_size, const void* load_data)noexcept;

		inline const unsigned int& reference() const noexcept { return m_Reference; }

	private:
		unsigned int m_Reference = 0;
	};
}