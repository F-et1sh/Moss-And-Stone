#include "forpch.h"
#include "Prefab.h"

FE2D::Prefab::Prefab(Entity entity) {
	FOR_COMPONENTS_HELPER::collect_from_entity(entity, this->m_Components);
}

bool FE2D::Prefab::LoadFromFile(const std::filesystem::path& file_path) {
	std::ifstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open file"); // there is no need of logging file path
		return false;
	}

	json j;
	file >> j;

	auto components = SceneSerializer::DeserializeComponents(j);
	for (auto& comp : components) {
		std::visit([&](auto&& c) {
			using T = std::decay_t<decltype(c)>;
			AddComponent<T>(std::move(c));
			}, comp);
	}

	return true;
}

void FE2D::Prefab::UploadToFile(const std::filesystem::path& file_path) const {
	std::ofstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open file\nPath : " << file_path.string().c_str());
		return;
	}
	
	json j = SceneSerializer::SerializeComponents(*this);

	file << j;
}

Entity FE2D::Prefab::CreateEntity(Scene& scene) {
	Entity e = { scene.getRegistry().create(), &scene };
	for (auto& comp : m_Components) {
		std::visit([&](auto& c) {
			using T = std::decay_t<decltype(c)>;
			e.AddComponent<T>(c);
			}, comp);
	}
	e.GetComponent<IDComponent>().id = FE2D::UUID(); // generate new UUID for Entity
	if (e.HasComponent<NativeScriptComponent>()) {
		auto& script = e.GetComponent<NativeScriptComponent>();
		if (script.instance) {
			script.instance->setContext(e);
		}
	}
	scene.EmplaceEntity(e);
	return e;
}

void FE2D::Prefab::OnEditorDraw(IMGUI& imgui) {

}
