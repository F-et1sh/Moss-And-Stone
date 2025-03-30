#include "forpch.h"
#include "UCameraComponent.h"

FOR_COMPONENT_REGISTER(UCameraComponent, "Camera")

json FE2D::UCameraComponent::Serialize() const {
	json j;
	Serializable::save_vec2(m_VisionSize, j, "Vision Size");
	return j;
}

void FE2D::UCameraComponent::Deserialize(const json& j) {
	Serializable::load_vec2(m_VisionSize, j, "Vision Size");
}

void FE2D::UCameraComponent::show_properties() {
	FOR_IMGUI.DragVector2("Vision Size", m_VisionSize);
}
