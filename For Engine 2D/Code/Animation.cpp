#include "forpch.h"
#include "Animation.h"

void FE2D::Animation::Release() {
	m_Frames.clear();
}

bool FE2D::Animation::LoadFromFile(const std::filesystem::path& file_path) {
	this->Release();

	std::ifstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to open a file of an Animation" << "\nPath : " << file_path);
		this->Release();
		return false;
	}
	
	json j;
	file >> j;

	if (!j.contains("Frames") || !j.contains("IsLooped")) {
		SAY("ERROR : Failed to load an Animation\nBroken file" << "\nPath : " << file_path);
		this->Release();
		return false;
	}

	SceneSerializer::load_vector(m_Frames, j, "Frames", [](FE2D::UUID& e, const json& j) {
		e = j.get<std::string>();
		});

	m_IsLooped = j["IsLooped"].get<bool>();

	return true;
}

void FE2D::Animation::CreateSource(const std::filesystem::path& file_path)const {
	if (file_path.empty()) {
		SAY("ERROR : Failed to create a source for an Animation\nEmpty file path");
		return;
	}
	
	std::ofstream file(file_path);
	if (!file.good()) {
		SAY("ERROR : Failed to create a source for an Animation" << "\nPath : " << file_path);
		return;
	}
	json j;
	SceneSerializer::save_vector(m_Frames, j, "Frames", [](const FE2D::UUID& e, json& j) { j = e.ToString(); });
	j["IsLooped"] = m_IsLooped;

#ifdef _DEBUG
	file << j.dump(4);
#else
	file << j;
#endif
}

void FE2D::Animation::OnEditorDraw(IMGUI& imgui) {
	ImGui::Text("Animation");
	imgui.CheckBox("Is Looped", m_IsLooped);
}

json FE2D::Animation::Serialize() const {
	return json();
}

void FE2D::Animation::Deserialize(const json& j) {
	// ..
}
