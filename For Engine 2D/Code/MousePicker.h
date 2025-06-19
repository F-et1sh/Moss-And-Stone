#pragma once
#include "Shader.h"

namespace FE2D {
    class FOR_API MousePicker {
    public:
        MousePicker() = default;
        ~MousePicker();

        inline void Initialize(const vec2& resolution)noexcept;
        inline void Bind()const noexcept;
        inline void Unbind()const noexcept;

        inline const unsigned int reference()const noexcept { return m_Reference; }
        inline const unsigned int texture_reference()const noexcept { return m_TextureReference; }
        inline const unsigned int depth_buffer_reference()const noexcept { return m_DepthBuffer; }

        inline const vec2 get_texture_size()const noexcept { return vec2(m_Width, m_Height); }

        int ReadPixel(const vec2& mouse_pos);

        Shader& getShader()noexcept { return m_Shader; }
        UniformBuffer& getUniformBuffer()noexcept { return m_UniformBuffer; }

        inline operator unsigned int& () noexcept { return m_Reference; }
        inline operator unsigned int* () noexcept { return &m_Reference; }
    private:
        unsigned int m_Reference = 0;
        unsigned int m_TextureReference = 0;
        unsigned int m_DepthBuffer = 0;
    private:
        unsigned int m_Width = 0;
        unsigned int m_Height = 0;

    private:
        Shader m_Shader;
        UniformBuffer m_UniformBuffer;
    };
}