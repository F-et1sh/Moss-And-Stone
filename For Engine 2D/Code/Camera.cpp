#include "forpch.h"
#include "Camera.h"

void FE2D::Camera::UpdateView()noexcept {
    if (!m_IsChanged) return;

    m_View = scale(mat4(1), vec3(vec2(m_Zoom), 1.0f));
    m_View = translate(m_View, vec3(1) - vec3(m_Position, 0.0f));

    m_IsChanged = false;
}
