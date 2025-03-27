#include "forpch.h"
#include "Framebuffer.h"

inline void FE2D::Framebuffer::Delete() noexcept {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureReference, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    {
        glCreateRenderbuffers(1, &m_DepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        SAY("ERROR : Failed to Initialize framebuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void FE2D::Framebuffer::Bind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Reference);
}

inline void FE2D::Framebuffer::Unbind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
