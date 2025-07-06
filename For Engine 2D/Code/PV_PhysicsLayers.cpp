#include "forpch.h"
#include "PV_PhysicsLayers.h"

json FE2D::PV_PhysicsLayers::Serialize()const {
    json j;
    json j_array = json::array();
    for (uint8_t i = 0; i < FOR_PHYSICS_LAYERS_COUNT; i++) {
        json j_element;
        j_element["name"] = m_Layers[i].name;
        j_element["mask"] = m_Layers[i].mask;
        
        j_array.emplace_back(j_element);
    }
    j["array"] = j_array;
    j["next"] = m_NextAvailableBit;
    return j;
}

void FE2D::PV_PhysicsLayers::Deserialize(const json& j) {
    const json& j_array = j["array"];
    for (uint8_t i = 0; i < FOR_PHYSICS_LAYERS_COUNT; i++) {
        m_Layers[i].name = j_array[i]["name"].get<std::string>();
        m_Layers[i].mask = j_array[i]["mask"].get<uint32_t>();
    }
    m_NextAvailableBit = j["next"].get<uint32>();
}

bool FE2D::PV_PhysicsLayers::createLayer(const std::string& name) {
    if (m_NextAvailableBit >= 32) return false;

    m_Layers[m_NextAvailableBit] = { name, 1u << m_NextAvailableBit };
    m_NextAvailableBit++;
    return true;
}

bool FE2D::PV_PhysicsLayers::removeLayer(int id) {
    if (id < 0 || id >= FOR_PHYSICS_LAYERS_COUNT) return false;

    m_Layers[id].name.clear();
    m_Layers[id].mask = 0;

    uint32_t bit = 1u << id;
    for (int i = 0; i < FOR_PHYSICS_LAYERS_COUNT; i++)
        m_Layers[i].mask &= ~bit;

    return true;
}