#include "forpch.h"
#include "Animation.h"

void FE2D::Animation::Release() {
	m_Frames.clear();
}

bool FE2D::Animation::LoadFromFile(const std::filesystem::path& file_path) {
	this->Release();

	/*std::ifstream file(file_path);

	if (!file.good()) {
		SAY(L"ERROR : Failed to load an animation" <<
			L"\nPath : " << file_path
		);

		this->Release();
	}

	json j;
	file >> j;
	
	SceneSerializer::load_vector(m_Frames, j, "frames", [](size_t& e, const json& j) {
		e = j.get<size_t>();
		});

	SceneSerializer::load_value(m_IsLooped, j, "is_looped");

	file.close();*/
	return true;
}

void FE2D::Animation::OnEditorDraw(IMGUI& imgui) {
	ImGui::Text("Animation");
}

json FE2D::Animation::Serialize() const {
	json j;
	SceneSerializer::save_vector(m_Frames, j, "frames", [](const size_t& e, json& j) {
		j = e;
		});

	SceneSerializer::save_value(m_IsLooped, j, "is_looped");
	return j;
}

void FE2D::Animation::Deserialize(const json& j) {
	// same process. Use LoadFromFile() instead
}

Animation::texture FE2D::Animation::getFrameTexture(frame frameIndex) const {
	if (m_IsLooped) {
		//frameIndex = frameIndex % m_Frames.size();
	}
	else {
		//frameIndex = min(frameIndex, m_Frames.size() - 1);
	}
	//return m_Frames[frameIndex];
	return 0;
}
