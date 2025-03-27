#include "forpch.h"
#include "USpriteComponent.h"

FOR_COMPONENT_REGISTER(USpriteComponent, "Sprite Renderer")

json FE2D::USpriteComponent::Serialize() const {
	json j;
	Serializable::save_value(m_TextureIndex, j, "Texture Index");
	Serializable::save_vec4(m_TextureCoords, j, "Texture Coords");
	Serializable::save_value(m_FlipX, j, "Flip X");
	Serializable::save_value(m_FlipY, j, "Flip Y");
	return j;
}

void FE2D::USpriteComponent::Deserialize(const json& j) {
	Serializable::load_value(m_TextureIndex, j, "Texture Index");
	Serializable::load_vec4(m_TextureCoords, j, "Texture Coords");
	Serializable::load_value(m_FlipX, j, "Flip X");
	Serializable::load_value(m_FlipY, j, "Flip Y");
}

void FE2D::USpriteComponent::show_properties() {
	vec2 xy = vec2(m_TextureCoords.x, m_TextureCoords.y);
	FOR_IMGUI.DragVector2("XY", xy);

	vec2 zw = vec2(m_TextureCoords.z, m_TextureCoords.w);
	FOR_IMGUI.DragVector2("ZW", zw);

	m_TextureCoords = vec4(xy, zw);

	FOR_IMGUI.SelectTexture("Sprite Texture", m_TextureIndex);

	FOR_IMGUI.CheckBox("Flip X", m_FlipX);
	FOR_IMGUI.CheckBox("Flip Y", m_FlipY);
}