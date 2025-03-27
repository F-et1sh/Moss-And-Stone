#include "forpch.h"
#include "VertexBuffer.h"

inline void VertexBuffer::Delete() noexcept { glDeleteBuffers(1, &m_Reference); }
inline void VertexBuffer::Create() noexcept { glCreateBuffers(1, &m_Reference); }
inline void VertexBuffer::Bind() const noexcept { glBindBuffer(GL_ARRAY_BUFFER, m_Reference); }
inline void VertexBuffer::Unbind() const noexcept { glBindBuffer(GL_ARRAY_BUFFER, 0); }

inline void VertexBuffer::BufferData(long long size, const void* data, int usage) const noexcept {
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

inline void FE2D::VertexBuffer::BufferSubData(long long offset, long long size, const void* data) const noexcept {
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}
