#include "forpch.h"
#include "SceneSerializer.h"

bool FE2D::SceneSerializer::Serialize(const std::filesystem::path& filepath) {
	std::ofstream file(filepath);
	if (!file.good()) {
		SAY("WARNING : Failed to Serialize a scene with JSON\nFile path : " << filepath);
		return false;
	}

	json j_scene;

	SerializeSceneInfo(j_scene["SceneInfo"]);

	j_scene["Entities"] = json::array();

	entt::registry& registry = m_Scene->getRegistry();

	registry.view<IDComponent>().each([&](auto entity, auto& uuid) {
		Entity load_entity = { entity, m_Scene };
		
		json j_entity;
		SerializeEntity(j_entity, load_entity);

		j_scene["Entities"].push_back(j_entity);
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
		uint64_t uuid = 0;
		SceneSerializer::load_value(uuid, j_entity, "UUID");

		std::string name;
		json tagComponent = j_entity["TagComponent"];
		SceneSerializer::load_value(name, tagComponent, "tag");

		if (!uuid || name.empty()) {
			SAY("WARNING : Failed to deserialize an entity" << 
				"\nUUID : " << uuid << 
				"\nName : " << name.c_str());
			continue;
		}

		// on initialization every entity already have :
		// - IDComponent
		// - TagComponent
		// - TransformComponent
		// - RelationshipComponent
		Entity deserializedEntity = m_Scene->CreateEntityWithUUID(UUID(uuid), name);

		if (j_entity.contains("TransformComponent")) {
			TransformComponent& component = deserializedEntity.GetComponent<TransformComponent>(); // entities always have TransformComponent

			json j_component = j_entity["TransformComponent"];

			SceneSerializer::load_vec2(component.position, j_component, "position");
			SceneSerializer::load_value(component.layer, j_component, "layer");
			SceneSerializer::load_value(component.auto_sort, j_component, "auto_sort");
			SceneSerializer::load_vec2(component.scale, j_component, "scale");
			SceneSerializer::load_value(component.rotation, j_component, "rotation");
			SceneSerializer::load_vec2(component.origin, j_component, "origin");
		}

		if (j_entity.contains("SpriteComponent")) {
			SpriteComponent& component = deserializedEntity.AddComponent<SpriteComponent>();

			json j_component = j_entity["SpriteComponent"];

			SceneSerializer::load_resource_id(component.texture, j_component, "texture");
			SceneSerializer::load_value(component.flip_x, j_component, "flip_x");
			SceneSerializer::load_value(component.flip_y, j_component, "flip_y");
		}

		if (j_entity.contains("RelationshipComponent")) {
			RelationshipComponent& component = deserializedEntity.GetComponent<RelationshipComponent>(); // entities always have RelationshipComponent

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
			CameraComponent& component = deserializedEntity.AddComponent<CameraComponent>();

			json j_component = j_entity["CameraComponent"];

			SceneSerializer::load_vec4(component.clear_color, j_component, "clear_color");
		}

		if (j_entity.contains("PlayerComponent")) {
			PlayerComponent& component = deserializedEntity.AddComponent<PlayerComponent>();

			json j_component = j_entity["PlayerComponent"];

			 // ..
		}

		if (j_entity.contains("VelocityComponent")) {
			VelocityComponent& component = deserializedEntity.AddComponent<VelocityComponent>();

			json j_component = j_entity["VelocityComponent"];
			SceneSerializer::load_vec2(component.velocity, j_component, "velocity");
		}

		if (j_entity.contains("ColliderComponent")) {
			ColliderComponent& component = deserializedEntity.AddComponent<ColliderComponent>();

			json j_component = j_entity["ColliderComponent"];
			SceneSerializer::load_vec2(component.position, j_component, "position");
			SceneSerializer::load_vec2(component.size, j_component, "size");
		}

		if (j_entity.contains("AnimatorComponent")) {
			AnimatorComponent& component = deserializedEntity.AddComponent<AnimatorComponent>();

			json j_component = j_entity["AnimatorComponent"];
			SceneSerializer::load_value(component.current_animation, j_component, "current_animation");
			SceneSerializer::load_value(component.time, j_component, "time");
			SceneSerializer::load_vector(component.animations, j_component, "animations", [](size_t& e, const json& j){
				e = j.get<size_t>();
				});
		}
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

	if (entity.HasComponent<PlayerComponent>()) {
		auto& component = entity.GetComponent<PlayerComponent>();

		json j_component;
		// ..

		j["PlayerComponent"] = j_component;
	}

	if (entity.HasComponent<VelocityComponent>()) {
		auto& component = entity.GetComponent<VelocityComponent>();

		json j_component;
		SceneSerializer::save_vec2(component.velocity, j_component, "velocity");

		j["VelocityComponent"] = j_component;
	}

	if (entity.HasComponent<ColliderComponent>()) {
		auto& component = entity.GetComponent<ColliderComponent>();

		json j_component;
		SceneSerializer::save_vec2(component.position, j_component, "position");
		SceneSerializer::save_vec2(component.size, j_component, "size");

		j["ColliderComponent"] = j_component;
	}

	if (entity.HasComponent<AnimatorComponent>()) {
		auto& component = entity.GetComponent<AnimatorComponent>();

		json j_component;
		SceneSerializer::save_value(component.current_animation, j_component, "current_animation");
		SceneSerializer::save_value(component.time, j_component, "time");
		SceneSerializer::save_vector(component.animations, j_component, "animations", [](const size_t& e, json& j){
			j = e;
			});

		j["AnimatorComponent"] = j_component;
	}
}

void FE2D::SceneSerializer::SerializeSceneInfo(json& j) {
	j["SceneIndex"] = m_Scene->getIndex();
	// ..
}
