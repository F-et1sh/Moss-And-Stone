#include "forpch.h"
#include "UCameraComponent.h"

FOR_COMPONENT_REGISTER(UCameraComponent, "Camera")

json FE2D::UCameraComponent::Serialize() const {
	json j;
	Serializable::save_vec2(m_VisionSize, j, "Vision Size");
	Serializable::save_vec2(m_Position, j, "Position");
	return j;
}

void FE2D::UCameraComponent::Deserialize(const json& j) {
	Serializable::load_vec2(m_VisionSize, j, "Vision Size");
	Serializable::load_vec2(m_Position, j, "Position");
}

void FE2D::UCameraComponent::show_properties() {
	FOR_IMGUI.DragVector2("Vision Size", m_VisionSize);
	FOR_IMGUI.DragVector2("Position", m_Position);
}
