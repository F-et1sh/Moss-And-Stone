#include "forpch.h"
#include "Camera.h"

void FE2D::Camera::UpdateView() {
	if (!m_IsChanged)
		return;

	m_View = translate(mat4(1.0f), vec3(m_Position, 0.0f));

	m_IsChanged = false;
}
