#include "forpch.h"
#include "PathManager.h"

void FE2D::PathManager::init(const char* argv0, bool is_editor) {
	if (!argv0)
		FOR_RUNTIME_ERROR("Failed to Initialize PathManager\nargv0 was nullptr");

	try {
		m_ExecutablePath = std::filesystem::canonical(argv0).parent_path();

		const std::filesystem::path solution_path =
			m_ExecutablePath.
			parent_path(). // exit from the platform folder
			parent_path(); // exit from the configuration folder

		const static std::filesystem::path assets_folder_name = L"Files";

		const std::filesystem::path editor_assets_folder_path = solution_path / assets_folder_name;
		const std::filesystem::path application_assets_folder_path = m_ExecutablePath / assets_folder_name;

		if (is_editor) {
			m_AssetsPath = editor_assets_folder_path;

			if (!std::filesystem::exists(m_AssetsPath)) {
				m_AssetsPath = application_assets_folder_path;
			}
		}
		else {
			m_AssetsPath = application_assets_folder_path;
		}

		if (std::filesystem::exists(editor_assets_folder_path)) {

			std::filesystem::remove_all(application_assets_folder_path);
			FE2D::copy_if_new(editor_assets_folder_path, application_assets_folder_path);
		}
	}
	catch (const std::exception& e) {
		FOR_RUNTIME_ERROR("Failed to Initialize PathManager\n" + std::string(e.what()));
	}
}
