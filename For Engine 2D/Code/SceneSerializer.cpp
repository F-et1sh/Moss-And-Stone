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

		json j_entity;
		SerializeEntity(j_entity, load_entity);

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

	// deserialize scriptable entities after other entities
	std::vector<std::pair<json, Entity>> scriptable_entities;

	entt::registry& registry = m_Scene->getRegistry();

	for (auto& j_entity : j["Entities"]) {
		uint64_t uuid = 0;
		SceneSerializer::load_value(uuid, j_entity, "UUID");

		std::string name;
		json tagComponent = j_entity["TagComponent"];
		SceneSerializer::load_value(name, tagComponent, "tag");

		if (!uuid || name.empty()) {
			SAY("WARNING : Failed to deserialize an entity. UUID or name are not valid" <<
				"\nUUID : " << uuid <<
				"\nName : " << name.c_str());
			continue;
		}

		// on initialization every entity already have :
		// - IDComponent
		// - TagComponent
		// - TransformComponent
		// - RelationshipComponent
		Entity deserialized_entity = m_Scene->CreateEntityWithUUID(UUID(uuid), name);

		if (j_entity.contains("TransformComponent")) {
			TransformComponent& component = deserialized_entity.GetComponent<TransformComponent>(); // entities always have TransformComponent

			json j_component = j_entity["TransformComponent"];

			SceneSerializer::load_vec2(component.position, j_component, "position");
			SceneSerializer::load_value(component.layer, j_component, "layer");
			SceneSerializer::load_value(component.auto_sort, j_component, "auto_sort");
			SceneSerializer::load_vec2(component.scale, j_component, "scale");
			SceneSerializer::load_value(component.rotation, j_component, "rotation");
			SceneSerializer::load_vec2(component.origin, j_component, "origin");
		}

		if (j_entity.contains("SpriteComponent")) {
			SpriteComponent& component = deserialized_entity.AddComponent<SpriteComponent>();

			json j_component = j_entity["SpriteComponent"];

			SceneSerializer::load_resource_id(component.texture, j_component, "texture");
			SceneSerializer::load_vec4(component.frame, j_component, "frame");
			SceneSerializer::load_value(component.flip_x, j_component, "flip_x");
			SceneSerializer::load_value(component.flip_y, j_component, "flip_y");
		}

		if (j_entity.contains("RelationshipComponent")) {
			RelationshipComponent& component = deserialized_entity.GetComponent<RelationshipComponent>(); // entities always have RelationshipComponent

			json j_component = j_entity["RelationshipComponent"];

			if (j_component.contains("parent")) {
				std::uint64_t temp_id;
				SceneSerializer::load_value(temp_id, j_component, "parent");
				component.parent = UUID(temp_id);
			}

			if (j_component.contains("children")) {
				component.children.clear();
				component.children.reserve(j_component["children"].size());
				for (const auto& j_child : j_component["children"]) {
					component.children.emplace_back(UUID(j_child.get<std::uint64_t>()));
				}
			}
		}

		if (j_entity.contains("CameraComponent")) {
			CameraComponent& component = deserialized_entity.AddComponent<CameraComponent>();

			json j_component = j_entity["CameraComponent"];

			SceneSerializer::load_vec4(component.clear_color, j_component, "clear_color");
		}

		if (j_entity.contains("PhysicsComponent")) {
			PhysicsComponent& component = deserialized_entity.AddComponent<PhysicsComponent>();

			json j_component = j_entity["PhysicsComponent"];
			SceneSerializer::load_vec2(component.position, j_component, "position");
			SceneSerializer::load_vec2(component.size, j_component, "size");
			SceneSerializer::load_value(component.bounce_factor, j_component, "bounce_factor");
			SceneSerializer::load_value(component.mass, j_component, "mass");
			SceneSerializer::load_value(component.is_trigger, j_component, "is_trigger");
			SceneSerializer::load_value(component.is_static, j_component, "is_static");
			SceneSerializer::load_value(component.velocity_dying, j_component, "velocity_dying");
		}

		if (j_entity.contains("AnimatorComponent")) {
			AnimatorComponent& component = deserialized_entity.AddComponent<AnimatorComponent>();

			json j_component = j_entity["AnimatorComponent"];
			SceneSerializer::load_array(component.animations, j_component, "animations", [](const json& j) -> std::pair<vec2, ResourceID<Animation>> {
				std::pair<vec2, ResourceID<Animation>> pair;
				SceneSerializer::load_vec2(pair.first, j, "coord");
				SceneSerializer::load_resource_id(pair.second, j, "animation");
				return pair;
				});
		}

		if (j_entity.contains("NativeScriptComponent")) {
			scriptable_entities.emplace_back(j_entity["NativeScriptComponent"], deserialized_entity);
		}
	}

	for (auto [j_component, entity] : scriptable_entities) {
		NativeScriptComponent& component = entity.AddComponent<NativeScriptComponent>();

		SceneSerializer::load_value(component.script_name, j_component, "script_name");
		if (!ScriptFactory::Instance().GetRegisteredScripts().contains(component.script_name)) {
			SAY("WARNING : Failed to deserialize a scriptable entity" <<
				"\nFailed to find script name in the list. Using empty name" <<
				"\nName : " << component.script_name.c_str());
			component.script_name.clear();
			continue;
		}

		component.instance = ScriptFactory::Instance().CreateScript(component.script_name, entity);
		component.instance->Deserialize(j_component["script_data"]);
	}

	// deserialize scene info after entities
	if (!DeserializeSceneInfo(j)) {
		SAY("ERROR : Failed to deserialize scene info\nFile path : " << full_path);
		return false;
	}

	return true;
}

void FE2D::SceneSerializer::SerializeEntity(json& j, Entity entity) {
	FOR_ASSERT(entity.HasComponent<IDComponent>(), "Cannot serialize an entity without IDComponent");

	uint64_t uuid = entity.GetComponent<IDComponent>().id.get();
	SceneSerializer::save_value(uuid, j, "UUID");

	if (entity.HasComponent<TagComponent>()) {
		auto& component = entity.GetComponent<TagComponent>();

		json j_component;
		SceneSerializer::save_value(component.tag, j_component, "tag");

		j["TagComponent"] = j_component;
	}

	if (entity.HasComponent<TransformComponent>()) {
		auto& component = entity.GetComponent<TransformComponent>();

		json j_component;
		SceneSerializer::save_vec2(component.position, j_component, "position");
		SceneSerializer::save_value(component.layer, j_component, "layer");
		SceneSerializer::save_value(component.auto_sort, j_component, "auto_sort");
		SceneSerializer::save_vec2(component.scale, j_component, "scale");
		SceneSerializer::save_value(component.rotation, j_component, "rotation");
		SceneSerializer::save_vec2(component.origin, j_component, "origin");

		j["TransformComponent"] = j_component;
	}

	if (entity.HasComponent<SpriteComponent>()) {
		auto& component = entity.GetComponent<SpriteComponent>();

		json j_component;
		SceneSerializer::save_resource_id(component.texture, j_component, "texture");
		SceneSerializer::save_vec4(component.frame, j_component, "frame");
		SceneSerializer::save_value(component.flip_x, j_component, "flip_x");
		SceneSerializer::save_value(component.flip_y, j_component, "flip_y");

		j["SpriteComponent"] = j_component;
	}

	if (entity.HasComponent<RelationshipComponent>()) {
		auto& component = entity.GetComponent<RelationshipComponent>();

		json j_component;
		SceneSerializer::save_value(component.parent.get(), j_component, "parent");

		json children_array = json::array();
		for (const auto& child : component.children) {
			children_array.push_back(child.get());
		}
		j_component["children"] = std::move(children_array);

		j["RelationshipComponent"] = j_component;
	}

	if (entity.HasComponent<CameraComponent>()) {
		auto& component = entity.GetComponent<CameraComponent>();

		json j_component;
		SceneSerializer::save_vec4(component.clear_color, j_component, "clear_color");

		j["CameraComponent"] = j_component;
	}

	if (entity.HasComponent<PhysicsComponent>()) {
		auto& component = entity.GetComponent<PhysicsComponent>();

		json j_component;
		SceneSerializer::save_vec2(component.position, j_component, "position");
		SceneSerializer::save_vec2(component.size, j_component, "size");
		SceneSerializer::save_value(component.bounce_factor, j_component, "bounce_factor");
		SceneSerializer::save_value(component.mass, j_component, "mass");
		SceneSerializer::save_value(component.is_trigger, j_component, "is_trigger");
		SceneSerializer::save_value(component.is_static, j_component, "is_static");
		SceneSerializer::save_value(component.velocity_dying, j_component, "velocity_dying");

		j["PhysicsComponent"] = j_component;
	}

	if (entity.HasComponent<AnimatorComponent>()) {
		auto& component = entity.GetComponent<AnimatorComponent>();

		json j_component;
		SceneSerializer::save_array(component.animations, j_component, "animations", [](const std::pair<vec2, ResourceID<Animation>>& e) -> json {
			json j;
			SceneSerializer::save_vec2(e.first, j, "coord");
			SceneSerializer::save_resource_id(e.second, j, "animation");
			return j;
			});

		j["AnimatorComponent"] = j_component;
	}

	if (entity.HasComponent<NativeScriptComponent>()) {
		auto& component = entity.GetComponent<NativeScriptComponent>();

		json j_component;
		SceneSerializer::save_value(component.script_name, j_component, "script_name");
		SceneSerializer::save_value(component.instance ? component.instance->Serialize() : json(), j_component, "script_data");

		j["NativeScriptComponent"] = j_component;
	}
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