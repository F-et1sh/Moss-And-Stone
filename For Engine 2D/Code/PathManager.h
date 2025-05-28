#pragma once

namespace FE2D {
	class FOR_API PathManager {
	public:
		PathManager(const PathManager&) = delete;
		PathManager& operator=(const PathManager&) = delete;

		void init(const char* argv0, bool is_editor);

		static PathManager& Instance() {
			static PathManager pathManager;	
			return pathManager;
		}

	public:
		inline const std::filesystem::path get_executable_path()const noexcept {
			return m_ExecutablePath;
		}
		
		inline const std::filesystem::path get_assets_path() const noexcept {
			return m_AssetsPath;
		}

		inline const std::filesystem::path get_scenes_path()const noexcept {
			return this->get_assets_path() / L"Scenes";
		}

		inline const std::filesystem::path get_resources_path()const noexcept {
			return this->get_assets_path() / L"Resources";
		}

		inline const std::filesystem::path get_fallbacks_path()const noexcept {
			return this->get_assets_path() / L"FE2D" / L"Fallbacks";
		}

		inline const std::filesystem::path get_shaders_path()const noexcept {
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