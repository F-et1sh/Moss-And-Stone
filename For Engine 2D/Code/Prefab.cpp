#include "forpch.h"
#include "Prefab.h"

bool FE2D::Prefab::LoadFromFile(const std::filesystem::path& file_path) {
	std::ifstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open file"); // there is no need of file path
		return false;
	}

	json j;
	file >> j;

	//SceneSerializer::DeserializeEntity(j, this)

	return true;
}

void FE2D::Prefab::UplopadToFile(const std::filesystem::path& file_path) const
{
}

void FE2D::Prefab::OnEditorDraw(IMGUI& imgui) {

}
