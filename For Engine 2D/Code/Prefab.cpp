#include "forpch.h"
#include "Prefab.h"

FE2D::Prefab::Prefab(Entity entity) {
	FOR_COMPONENTS_HELPER::collect_from_entity(entity, main_entity().m_Components);

	auto children = entity.GetChildren();
	if (children.empty()) return;
	
	for (auto child : children) {
		PrefabEntity entity;
		FOR_COMPONENTS_HELPER::collect_from_entity(child, entity.m_Components);
		m_Entities.emplace_back(std::move(entity));
	}
}

bool FE2D::Prefab::LoadFromFile(const std::filesystem::path& file_path) {
	std::ifstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open file\nPath : " << file_path);
		return false;
	}

	json j;
	file >> j;
	if (!j.is_array()) {
		SAY("ERROR : Failed to read JSON\nIt's not an array\nPath : " << file_path);
		return false;
	}

	for (const auto& entity : j) {
		auto components = SceneSerializer::DeserializeComponents(entity);
		PrefabEntity prefab_entity{ components };
		m_Entities.emplace_back(std::move(prefab_entity));
	}

	return true;
}

void FE2D::Prefab::UploadToFile(const std::filesystem::path& file_path) const {
	std::ofstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open file\nPath : " << file_path);
		return;
	}
	
	json j = json::array();
	for (auto entity : m_Entities) {
		j.emplace_back(SceneSerializer::SerializeComponents(entity));
	}

#ifdef _DEBUG
	file << j.dump(4);
#else
	file << j;
#endif
}

Entity FE2D::Prefab::CreateEntity(Scene& scene) {
	Entity main_entity;
	std::vector<Entity> entities_with_script;

	for (auto& e : m_Entities) {
		Entity entity = { scene.getRegistry().create(), &scene };
		for (auto& comp : e.m_Components) {
			std::visit([&](auto& c) {
				using T = std::decay_t<decltype(c)>;
				entity.AddComponent<T>(c);

				if constexpr (std::is_same_v<T, NativeScriptComponent>)
					entities_with_script.emplace_back(entity);

				}, comp);
		}
		
		auto& id_component = entity.GetComponent<IDComponent>();
		FE2D::UUID old_uuid = id_component.id;
		id_component.id = FE2D::UUID(); // generate new UUID for the Entity

		scene.EmplaceEntity(entity); // add the entity before setting context of the script

		if (!main_entity) main_entity = entity; // store the first entity
		else {
			if (scene.getEntityMap().find(old_uuid) != scene.getEntityMap().end())
				Entity::UnbindEntities(main_entity, scene.GetEntityByUUID(old_uuid));
			Entity::BindEntities(main_entity, entity);
		}
	}

	if (!main_entity) {
		SAY("ERROR : Failed to create an entity from prefab\nMain entity is not initialized. Returning empty entity");
		return {};
	}

	for (auto entity : entities_with_script)
		entity.GetComponent<NativeScriptComponent>().instance->setContext(entity);

	for (auto entity : entities_with_script)
		entity.GetComponent<NativeScriptComponent>().instance->OnAwake();

	return main_entity;
}

void FE2D::Prefab::OnEditorDraw(IMGUI& imgui) {
	// TODO : Create prefab editing
	
	// 04.07.2025 - 16:36 : maybe somehow in 2026 with C++26
}
