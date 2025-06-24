#include "forpch.h"
#include "SceneSerializer.h"

bool FE2D::SceneSerializer::Serialize(const std::filesystem::path& full_path) {
	std::ofstream file(full_path);
	if (!file.good()) {
		SAY("ERROR : Failed to serialize a scene with JSON\nFile path : " << full_path);
		return false;
	}

	json j;

	SerializeSceneInfo(j);

	j["Entities"] = json::array();

	entt::registry& registry = m_Scene->getRegistry();

	registry.view<IDComponent>().each([&](auto entity, auto& uuid) {
		Entity load_entity = { entity, m_Scene };

		json j_entity = SerializeComponents(load_entity);

		j["Entities"].push_back(j_entity);
		});

#ifdef _DEBUG
	file << j.dump(4); // 4 - to make the file more readable
#else
	file << j.dump();
#endif

	file.close();

	return true;
}

bool FE2D::SceneSerializer::Deserialize(const std::filesystem::path& full_path) {
	std::ifstream file(full_path);
	if (!file.good()) {
		SAY("ERROR : Failed to deserialize a scene with JSON\nFile path : " << full_path);
		return false;
	}

	json j;
	file >> j;
	file.close();

	entt::registry& registry = m_Scene->getRegistry();

	for (auto& j_entity : j["Entities"]) {
		Entity e{ m_Scene->getRegistry().create(), m_Scene };

		auto components = DeserialzieComponents(j_entity);
		for (auto& comp : components) {
			std::visit([&](auto&& c) {
				using T = std::decay_t<decltype(c)>;
				e.AddComponent<T>(std::move(c));
				}, comp);
		}

		m_Scene->EmplaceEntity(e);
	}

	// deserialize scene info after entities
	if (!DeserializeSceneInfo(j)) {
		SAY("ERROR : Failed to deserialize scene info\nFile path : " << full_path);
		return false;
	}

	return true;
}

void FE2D::SceneSerializer::SerializeSceneInfo(json& j) {
	json info;
	info["SceneIndex"] = m_Scene->getIndex();

	info["SpriteRendererSystem"] = m_Scene->m_SpriteRendererSystem->Serialize();
	info["PhysicsSystem"] = m_Scene->m_PhysicsSystem->Serialize();
	info["AnimationSystem"] = m_Scene->m_AnimationSystem->Serialize();

	j["SceneInfo"] = info;
}

bool FE2D::SceneSerializer::DeserializeSceneInfo(const json& j) {
	if (!j.contains("SceneInfo")) {
		SAY("ERROR : Failed to deserialize scene info. There is no \"SceneInfo\" in the JSON");
		return false;
	}

	json info = j["SceneInfo"];

	if (!info.contains("SceneIndex")) {
		SAY("ERROR : Failed to deserialize scene info. There is no \"SceneIndex\" in the JSON");
		return false;
	}

	SceneSerializer::load_value(m_Scene->m_Index, info, "SceneIndex");

	if (info.contains("SpriteRendererSystem"))      m_Scene->m_SpriteRendererSystem->Deserialize(info["SpriteRendererSystem"]);
	if (info.contains("PhysicsSystem"))             m_Scene->m_PhysicsSystem->Deserialize(info["PhysicsSystem"]);
	if (info.contains("AnimationSystem"))           m_Scene->m_AnimationSystem->Deserialize(info["AnimationSystem"]);

	return true;
}