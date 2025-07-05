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
		SAY("ERROR : Failed to open file"); // there is no need of logging file path
		return false;
	}

	json j;
	file >> j;
	if (!j.is_array()) {
		SAY("ERROR : Failed to read JSON\nIt's not an array"); // there is no need of logging file path
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
		SAY("ERROR : Failed to open file\nPath : " << file_path.string().c_str());
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

	for (auto& entity : m_Entities) {
		Entity e = { scene.getRegistry().create(), &scene };
		for (auto& comp : entity.m_Components) {
			std::visit([&](auto& c) {
				using T = std::decay_t<decltype(c)>;
				e.AddComponent<T>(c);
				}, comp);
		}
		
		auto& id_component = e.GetComponent<IDComponent>();
		FE2D::UUID old_uuid = id_component.id;
		id_component.id = FE2D::UUID(); // generate new UUID for the Entity

		scene.EmplaceEntity(e); // add the entity before setting context

		if (e.HasComponent<NativeScriptComponent>()) {
			auto& script = e.GetComponent<NativeScriptComponent>();
			if (script.instance) {
				script.instance->setContext(e);
			}
		}

		if (!main_entity) main_entity = e; // store the first entity
		else {
			if (scene.getEntityMap().find(old_uuid) != scene.getEntityMap().end())
				Entity::UnbindEntities(main_entity, scene.GetEntityByUUID(old_uuid));
			Entity::BindEntities(main_entity, e);
		}
	}

	if (!main_entity) {
		SAY("ERROR : Failed to create an entity from prefab\nMain entity is not initialized. Returning empty entity");
		return {};
	}

	return main_entity;
}

void FE2D::Prefab::OnEditorDraw(IMGUI& imgui) {
	// TODO : Create prefab editing
	
	// 04.07.2025 - 16:36 : maybe somehow in 2026 with C++26
}
