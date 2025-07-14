#pragma once

namespace FE2D {
	class FOR_API PathManager {
	public:
		FOR_CLASS_NONCOPYABLE(PathManager)

		void init(const char* argv0, bool is_editor);

		static PathManager& Instance() {
			static PathManager path_manager;	
			return path_manager;
		}

	public:
		inline const std::filesystem::path& get_executable_path()const noexcept {
			return m_ExecutablePath;
		}
		
		inline const std::filesystem::path& get_assets_path() const noexcept {
			return m_AssetsPath;
		}

		inline std::filesystem::path get_application_path()const {
			return this->get_assets_path() / L"Application";
		}

		inline std::filesystem::path get_cursors_path()const {
			return this->get_application_path() / "Cursors";
		}

		inline std::filesystem::path this_scenes_path()const {
			return this->get_assets_path() / L"Scenes";
		}

		inline std::filesystem::path get_resources_path()const {
			return this->get_assets_path() / L"Resources";
		}

		inline std::filesystem::path get_engine_data_path()const {
			return this->get_assets_path() / L"FE2D";
		}

		inline std::filesystem::path get_fallbacks_path()const {
			return this->get_engine_data_path() / L"Fallbacks";
		}

		inline std::filesystem::path get_shaders_path()const {
			return this->get_assets_path() / L"Shaders";
		}

	private:
		PathManager() = default;
		~PathManager() = default;

	private:
		std::filesystem::path m_ExecutablePath;
		std::filesystem::path m_AssetsPath;
	};
	inline static PathManager& FOR_PATH = PathManager::Instance();
}