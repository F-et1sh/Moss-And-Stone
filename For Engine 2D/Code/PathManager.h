#pragma once

namespace FE2D {
	class FOR_API PathManager {
	public:
		void init(const char* argv0, bool is_editor);

		static PathManager& Instance() {
			static PathManager pathManager;
			return pathManager;
		}

	public:

		constexpr std::filesystem::path get_executable_path()const noexcept {
			return m_ExecutablePath;
		}
		
		constexpr std::filesystem::path get_assets_path() const noexcept {
			return m_AssetsPath;
		}

		inline const std::filesystem::path get_scenes_path()const noexcept {
			return this->get_assets_path() / L"Scenes";
		}

		inline const std::filesystem::path get_resources_path()const noexcept {
			return this->get_assets_path() / L"Resources";
		}

		inline const std::filesystem::path get_shaders_path()const noexcept {
			return this->get_assets_path() / L"Shaders";
		}

	private:
		std::filesystem::path m_ExecutablePath;
		std::filesystem::path m_AssetsPath;
	};
	inline static PathManager& FOR_PATH = PathManager::Instance();
}