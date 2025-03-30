#include "forpch.h"
#include "Framebuffer.h"

inline void FE2D::Framebuffer::Delete() noexcept {
    this->Unbind();

    glDeleteRenderbuffers(1, &m_DepthBuffer);
    glDeleteFramebuffers(1, &m_Reference);
    glDeleteTextures(1, &m_TextureReference);
}

inline void FE2D::Framebuffer::Initialize(const vec2& resolution) noexcept {
    this->Delete();

    glCreateFramebuffers(1, &m_Reference);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Reference);

    m_Width = resolution.x;
    m_Height = resolution.y;

    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureReference);
        glBindTexture(GL_TEXTURE_2D, m_TextureReference);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
}

inline void FE2D::Framebuffer::Bind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Reference);
}

inline void FE2D::Framebuffer::Unbind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
