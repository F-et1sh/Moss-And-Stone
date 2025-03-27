#pragma once

namespace FE2D {
	class FOR_API VertexArray {
	public:
		VertexArray() = default;
		~VertexArray() = default;
	public:
		inline void Delete()noexcept;
		inline void Create()noexcept;
		inline void Bind()const noexcept;
		inline void Unbind()const noexcept;

		inline void EnableAttrib(const unsigned int index)const noexcept;
		inline void EnableVertexAttribArray(const unsigned int index)const noexcept;
		inline void AttribPointer(const unsigned int index, const int values_count, const unsigned int type, const bool normalized, const size_t stride, const void* offset)const noexcept;

		inline const unsigned int reference() const noexcept { return m_Reference; }

		inline operator unsigned int& () noexcept { return m_Reference; }
		inline operator unsigned int* () noexcept { return &m_Reference; }
	private:
		unsigned int m_Reference = 0;
	};
}