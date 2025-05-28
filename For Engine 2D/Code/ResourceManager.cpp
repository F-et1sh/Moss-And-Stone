#include "forpch.h"
#include "ResourceManager.h"

const std::vector<unsigned char> FE2D::ResourceManager::load_file_to_memory(const std::wstring& file_path) {
	std::ifstream file(file_path, std::ios::binary);
	if (!file)
		return std::vector<unsigned char>();
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

std::vector<std::filesystem::path> FE2D::ResourceManager::scan_folder(const std::filesystem::path& folder_path, ScanMode mode, size_t start_capacity) {
    if (!std::filesystem::exists(folder_path)) {
        SAY("ERROR : Failed to scan folder. It's not exist\nPath : " << folder_path);
        return {};
    }

    if (!std::filesystem::is_directory(folder_path)) {
        SAY("ERROR : Failed to scan folder. It's not a directory\nPath : " << folder_path);
        return {};
    }

    std::vector<std::filesystem::path> scanned;
    std::vector<std::filesystem::path> directories_to_process = { folder_path };
    scanned.reserve(start_capacity);

    while (!directories_to_process.empty()) {
        auto current_dir = directories_to_process.back();
        directories_to_process.pop_back();

        for (const auto& entry : std::filesystem::directory_iterator(current_dir)) {
            if (mode == ScanMode::BOTH)
                scanned.push_back(entry.path());
            else {
                if (entry.is_directory()) {
                    directories_to_process.push_back(entry.path());
                    if (mode == ScanMode::FOLDERS)
                        scanned.push_back(entry.path());
                }
                else if (mode == ScanMode::FILES)
                    scanned.push_back(entry.path());
            }
        }
    }
    return scanned;
}

void FE2D::ResourceManager::Release() {
    this->save_resources();
}

void FE2D::ResourceManager::Initialize() {
    this->load_fallback_resources();
    this->load_available_metadata();
}

void FE2D::ResourceManager::load_available_resources() {
    auto files = scan_folder(FOR_PATH.get_assets_path(), ResourceManager::ScanMode::FILES);
    for (const auto& file : files) {
        try {
            m_ResourceLoader.LoadResource(file);
        }
        catch (const std::exception& e) {
            SAY("WARNING : Failed to load resource" << "\nPath : " << file << "\nReason : " << e.what());
        }
    }
}

void FE2D::ResourceManager::load_available_metadata() {
    auto files = scan_folder(FOR_PATH.get_assets_path(), ResourceManager::ScanMode::FILES);
    for (const auto& file : files) {
        try {
            if (file.extension() != L".fs") continue;
            m_ResourceLoader.LoadMetadata(file);
        }
        catch (const std::exception& e) {
            SAY("WARNING : Failed to load resource" << "\nPath : " << file << "\nReason : " << e.what());
        }
    }
}

void FE2D::ResourceManager::load_fallback_resources() {
    m_ResourceLoader.LoadFallback(L"Texture.png");
}

void FE2D::ResourceManager::save_resources() {
    const auto& metadata_array = m_ResourceCache.get_metadata_array();
    for (const auto& metadata : metadata_array) {

        const FE2D::UUID uuid = metadata.first;
        const std::filesystem::path& path = FOR_PATH.get_assets_path() / metadata.second;

        m_ResourceLoader.CreateMetadata(path, uuid);
    }
}

FE2D::UUID FE2D::ResourceManager::GetResourceByPath(const std::filesystem::path& path) const {
    for (auto metadata : m_ResourceCache.get_metadata_array()) {
        if (metadata.second == path)
            return metadata.first;
    }

    return FE2D::UUID(0);
}
