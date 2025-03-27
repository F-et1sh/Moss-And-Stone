#include "forpch.h"
#include "UniformBuffer.h"

inline void UniformBuffer::release()noexcept { glDeleteBuffers(1, &m_Reference); }
inline void UniformBuffer::create()noexcept { glCreateBuffers(1, &m_Reference); }

inline void UniformBuffer::bindBlock(unsigned int block_index)noexcept {
	glBindBufferBase(GL_UNIFORM_BUFFER, block_index, m_Reference);
}

inline void UniformBuffer::bufferData(unsigned int load_size, const void* load_data, int update_mode)noexcept {
	glNamedBufferData(m_Reference, load_size, load_data, update_mode);
}

inline void UniformBuffer::bufferSubData(unsigned int load_offset, unsigned int load_size, const void* load_data) noexcept {
    glNamedBufferSubData(m_Reference, load_offset, load_size, load_data);
}
