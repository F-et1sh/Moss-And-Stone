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
    return j;
}

void FE2D::PV_PhysicsLayers::Deserialize(const json& j) {
    const json& j_array = j["array"];
    for (uint8_t i = 0; i < FOR_PHYSICS_LAYERS_COUNT; i++) {
        m_Layers[i].name = j_array[i]["name"].get<std::string>();
        m_Layers[i].mask = j_array[i]["mask"].get<uint32_t>();
    }
}

bool FE2D::PV_PhysicsLayers::createLayer(const std::string& name) {
    uint8_t id = get_last_index();
    if (id >= FOR_PHYSICS_LAYERS_COUNT) return false;

    m_Layers[id] = { name, 0 };
    return true;
}

bool FE2D::PV_PhysicsLayers::removeLayer(int id) {
    if (id < 0 || id >= FOR_PHYSICS_LAYERS_COUNT) return false;

    m_Layers[id].name.clear();
    m_Layers[id].mask = 0;

    for (int i = 0; i < FOR_PHYSICS_LAYERS_COUNT; i++)
        m_Layers[i].mask &= ~(1u << id);

    return true;
}

uint8_t FE2D::PV_PhysicsLayers::get_last_index()const {
    uint8_t i = 0;
    for (auto& info : m_Layers)
        if (!info.name.empty()) i++;
    return i;
}