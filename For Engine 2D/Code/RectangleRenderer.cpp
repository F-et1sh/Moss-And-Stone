#include "forpch.h"
#include "RectangleRenderer.h"

void FE2D::RectangleRenderer::Release() {

}

const bool FE2D::RectangleRenderer::Initialize() {
    m_Shader.Initialize(L"Files/Shaders/Default/Rectangle/Rectangle");
    
    m_VertexArray.Create();
    m_VertexArray.Bind();

    m_VertexBuffer.Create();
    m_VertexBuffer.Bind();
    m_VertexBuffer.BufferData(sizeof(vec2), nullptr, GL_STATIC_DRAW);

    m_VertexArray.AttribPointer(0, 2, GL_FLOAT, false, sizeof(vec2), nullptr);
    m_VertexArray.EnableAttrib(0);

    return true;
}

inline void FE2D::RectangleRenderer::RegisterRectangle(const Rectangle& load_rectangle) {

}

void FE2D::RectangleRenderer::RenderView() {

}
