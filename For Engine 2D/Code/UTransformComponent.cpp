#include "forpch.h"
#include "UTransformComponent.h"

FOR_COMPONENT_REGISTER(UTransformComponent, "Transform")

inline void FE2D::UTransformComponent::RecalculateTransform() noexcept {
    // scaling
    {
        m_Matrix = scale(vec3(m_Scale, 1.0f));
    }

    // rotation
    {
        // set origin
        m_Matrix *= translate(vec3(-m_Origin, 0));

        // apply rotation
        m_Matrix *= rotate(vec3(0.0f, 0.0f, m_Rotation * FOR_RADIAN));
        
        // reset origin
        m_Matrix *= translate(vec3(m_Origin, 0));
    }

    // translation
    {
        m_Matrix *= translate(vec3(m_Position,                             // position
            (m_AutoSort ? (-m_Position.y * std::abs(m_Scale.y) / 2) : 0) + // autoSorting
            m_Layer));                                                     // layer
    }
}

inline const mat4& FE2D::UTransformComponent::getTransform() noexcept {
    if (m_IsTransformChanged) {
        this->RecalculateTransform();
        m_IsTransformChanged = false;
    }
    return m_Matrix;
}

json FE2D::UTransformComponent::Serialize() const {
    json j;
    Serializable::save_value(m_IsStatic, j, "Static"  );
    Serializable::save_vec2 (m_Position, j, "Position");
    Serializable::save_value(m_Layer   , j, "Layer"   );
    Serializable::save_value(m_AutoSort, j, "AutoSort");
    Serializable::save_vec2 (m_Scale   , j, "Scale"   );
    Serializable::save_value(m_Rotation, j, "Rotation");
    Serializable::save_vec2 (m_Origin  , j, "Origin"  );
    return j;
}

void FE2D::UTransformComponent::Deserialize(const json& j) {
    Serializable::load_value(m_IsStatic, j, "Static"  );
    Serializable::load_vec2 (m_Position, j, "Position");
    Serializable::load_value(m_Layer   , j, "Layer"   );
    Serializable::load_value(m_AutoSort, j, "AutoSort");
    Serializable::load_vec2 (m_Scale   , j, "Scale"   );
    Serializable::load_value(m_Rotation, j, "Rotation");
    Serializable::load_vec2 (m_Origin  , j, "Origin"  );

    this->RecalculateTransform();
}

void FE2D::UTransformComponent::show_properties() {
    FOR_IMGUI.CheckBox("Is Static", m_IsStatic);
    ImGui::Spacing();
	
    if (m_IsStatic)
        return;

    FOR_IMGUI.DragVector2("Position", m_Position);
    ImGui::Spacing();

    FOR_IMGUI.DragInt("Layer", m_Layer);
    ImGui::Spacing();

    FOR_IMGUI.CheckBox("Auto Sort", m_AutoSort);
    ImGui::Spacing();

    FOR_IMGUI.DragVector2("Scale", m_Scale, 1.0f);
    ImGui::Spacing();

    FOR_IMGUI.DragFloat("Rotation", m_Rotation);
    ImGui::Spacing();

    FOR_IMGUI.DragVector2("Origin", m_Origin);
    ImGui::Spacing();

    m_IsTransformChanged = true;
}
