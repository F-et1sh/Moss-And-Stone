#pragma once

namespace FE2D {
	class FOR_API VertexBuffer {
	public:
		VertexBuffer() = default;
		~VertexBuffer() = default;
	public:
		inline void Delete()noexcept;
		inline void Create()noexcept;
		inline void Bind()const noexcept;
		inline void Unbind()const noexcept;

		inline void BufferData(long long size, const void* data, int usage)const noexcept;
		inline void BufferSubData(long long offset, long long size, const void* data) const noexcept;

		inline const unsigned int reference() const noexcept { return m_Reference; }

		inline operator unsigned int& () noexcept { return m_Reference; }
		inline operator unsigned int* () noexcept { return &m_Reference; }
	public:
		unsigned int m_Reference = 0;
	};
}