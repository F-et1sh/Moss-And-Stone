#include "forpch.h"
#include "MousePicker.h"

FE2D::MousePicker::~MousePicker() {
    this->Unbind();
    glDeleteRenderbuffers(1, &m_DepthBuffer);
    glDeleteFramebuffers(1, &m_Reference);
    glDeleteTextures(1, &m_TextureReference);
}

inline void FE2D::MousePicker::Initialize(const vec2& resolution) noexcept {
    glCreateFramebuffers(1, &m_Reference);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Reference);

    m_Width = resolution.x;
    m_Height = resolution.y;

    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureReference);
        glBindTexture(GL_TEXTURE_2D, m_TextureReference);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R32I,
            m_Width,
            m_Height,
            0,
            GL_RED_INTEGER,
            GL_INT,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureReference, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    {
        glCreateRenderbuffers(1, &m_DepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_UNDEFINED");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_UNSUPPORTED");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            SAY("ERROR : Framebuffer status is GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
            break;
        default:
            SAY("ERROR : Unknown framebuffer error");
            break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_Shader.Initialize(FOR_PATH.get_shaders_path() / L"Picker Default" / L"PickerDefault");
    m_UniformBuffer.create();
    m_UniformBuffer.bindBlock(UniformBuffer::MOUSE_PICKER);

    constexpr size_t entity_count        = 256;
    constexpr size_t texture_atlas_size  = sizeof(vec4);
    constexpr size_t texture_coords_size = sizeof(vec4);
    constexpr size_t matrix_size         = sizeof(mat4);
    constexpr size_t entity_hanle        = sizeof(vec4); // use vec4 instead of just int because of alignas of std140

    m_UniformBuffer.bufferData(texture_atlas_size + entity_count * (matrix_size + texture_coords_size + entity_hanle), nullptr, GL_DYNAMIC_DRAW);
}

inline void FE2D::MousePicker::Bind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Reference);
}

inline void FE2D::MousePicker::Unbind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int MousePicker::ReadPixel(const vec2& mouse_pos) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Reference);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    int entityHandle = 0;
    glReadPixels(static_cast<int>(mouse_pos.x), static_cast<int>(mouse_pos.y), 1, 1, GL_RED_INTEGER, GL_INT, &entityHandle);

    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return entityHandle;
}