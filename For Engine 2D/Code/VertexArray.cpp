#include "forpch.h"
#include "VertexArray.h"

inline void VertexArray::Delete() noexcept { glDeleteVertexArrays(1, &m_Reference); }
inline void VertexArray::Create() noexcept { glCreateVertexArrays(1, &m_Reference); }
inline void VertexArray::Bind() const noexcept { glBindVertexArray(m_Reference); }
inline void VertexArray::Unbind() const noexcept { glBindVertexArray(0); }

inline void VertexArray::EnableAttrib(const unsigned int index) const noexcept { glEnableVertexArrayAttrib(m_Reference, index); }
inline void VertexArray::EnableVertexAttribArray(const unsigned int index) const noexcept { glEnableVertexAttribArray(index); }
inline void VertexArray::AttribPointer(const unsigned int index, const int values_count, const unsigned int type, const bool normalized, const size_t stride, const void* offset) const noexcept { 
	glVertexAttribPointer(index, values_count, type, normalized, stride, offset); 
}