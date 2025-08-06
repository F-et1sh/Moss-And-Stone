#include "forpch.h"
#include "Tileset.h"

bool FE2D::Tileset::LoadFromFile(const std::filesystem::path& file_path) {
	std::ifstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to load tileset\nPath : " << file_path);
		return false;
	}

	json j;
	file >> j;

	if (!j.contains("tiles")) {
		SAY("ERROR : Failed to load tileset. JSON doens't contain array \"tiles\"\nPath : " << file_path);
		return false;
	}

	if (!j["tiles"].is_array()) {
		SAY("ERROR : Failed to load tileset. \"tiles\" is not array\nPath : " << file_path);
		return false;
	}

	const json& j_array = j["tiles"];
	for (const json& j_element : j_array) {
		vec4 uv = vec4(); // SceneSerializer::load_vec4 doesn't support ivec4
		ResourceID<Texture> texture_id;

		SceneSerializer::load_vec4(uv, j_element, "uv");
		SceneSerializer::load_resource_id(texture_id, j_element, "texture_id");

		this->addTile({ uv, texture_id });
	}

	return true;
}

void FE2D::Tileset::UploadToFile(const std::filesystem::path& file_path)const {
	std::ofstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to upload tileset\nPath : " << file_path);
		return;
	}

	json j;
	json j_array = json::array();

	for (const auto& tile : m_Tiles) {
		json j_element;
		SceneSerializer::save_vec4(tile.uv, j_element, "uv");
		SceneSerializer::save_resource_id(tile.texture_id, j_element, "texture_id");
		j_array.emplace_back(j_element);
	}

	j["tiles"] = j_array;

#ifdef _DEBUG
	file << j.dump(4);
#else
	file << j;
#endif
}