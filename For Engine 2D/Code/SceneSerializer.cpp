#include "forpch.h"
#include "SceneSerializer.h"

bool FE2D::SceneSerializer::Serialize(const std::filesystem::path& filepath) {
	std::ofstream file(filepath);
	if (!file.good()) {
		SAY("WARNING : Failed to Serialize a scene with JSON\nFile path : " << filepath);
		return false;
	}

	json j_scene;


	json j_sceneInfo;
	j_sceneInfo["SceneIndex"] = m_Scene->getIndex();

	j_scene["SceneInfo"] = j_sceneInfo;


	j_scene["Entities"] = json::array();

	entt::registry& registry = m_Scene->getRegistry();

	registry.view<IDComponent>().each([&](auto entity, auto& tagComponent) {

		json j;
		j["Components"] = json::array();

		for (auto& [id, entry] : ComponentFactory::Instance().getRegistredComponents()) {
			if (!entry.serializeFunc)
				continue;

			auto* storage = registry.storage(id);
			if (storage && storage->contains(entity)) {
				json j_component;
				j_component = entry.serializeFunc(registry, entity);
				
				j["Components"].push_back(j_component);
			}
		}

		j_scene["Entities"].push_back(j);

		});

#ifdef _DEBUG
	file << j_scene.dump(4); // 4 - to make the file more readable
#else
	file << j_scene;
#endif

	file.close();

	return true;
}

bool FE2D::SceneSerializer::Deserialize(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.good()) {
        SAY("WARNING : Failed to Deserialize a scene with JSON\nFile path : " << filepath);
        return false;
    }

    json j_scene;
    file >> j_scene;
    file.close();

	m_Scene->Release();

    entt::registry& registry = m_Scene->getRegistry();

    for (auto& j_entity : j_scene["Entities"]) {
        entt::entity entity = registry.create();

        for (auto& j_component : j_entity["Components"]) {
			for (auto& [id, entry] : ComponentFactory::Instance().getRegistredComponents()) {
				if (!entry.emplaceFunc || !entry.deserializeFunc)
					continue;

				if (!j_component.contains(entry.getNameFunc()))
					continue;

				entry.emplaceFunc(registry, entity);
				entry.deserializeFunc(registry, entity, j_component);
			}
        }

		m_Scene->EmplaceEntity({ entity, m_Scene });
    }

    return true;
}
