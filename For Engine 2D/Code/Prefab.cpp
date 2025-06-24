#include "forpch.h"
#include "Prefab.h"

bool FE2D::Prefab::LoadFromFile(const std::filesystem::path& file_path) {
	std::ifstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open file"); // there is no need of logging file path
		return false;
	}

	json j;
	file >> j;

	auto components = SceneSerializer::DeserialzieComponents(j);
	for (auto& comp : components) {
		std::visit([&](auto&& c) {
			using T = std::decay_t<decltype(c)>;
			AddComponent<T>(std::move(c));
			}, comp);
	}

	return true;
}

void FE2D::Prefab::UplopadToFile(const std::filesystem::path& file_path) const {
	std::ofstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open file\nPath : " << file_path.string().c_str());
		return;
	}
	
	json j = SceneSerializer::SerializeComponents(*this);

	file << j;
}

void FE2D::Prefab::OnEditorDraw(IMGUI& imgui) {

}
