#include "forpch.h"
#include "ResourceManager.h"

const std::vector<unsigned char> FE2D::ResourceManager::load_file_to_memory(const std::wstring& file_path) {
	std::ifstream file(file_path, std::ios::binary);
	if (!file)
		return std::vector<unsigned char>();
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

const std::vector<std::filesystem::path> FE2D::ResourceManager::scan_folder(
    const std::filesystem::path& folder_path,
    ScanMode mode,
    size_t start_capacity) {
    
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
    this->SaveResources();
    this->ClearCache();
}

void FE2D::ResourceManager::Initialize() {
    this->load_available_metadata();
    this->load_available_resources();
}

void FE2D::ResourceManager::load_available_resources() {
    auto files = scan_folder(FOR_PATH.get_assets_path(), ScanMode::FILES);
    for (const auto& path : files) {
        try {
            this->load_resource(path);
        }
        catch (const std::exception& e) {
            SAY("WARNING : Failed to load resouce\nPath : " << path << "\nException : " << e.what());
        }
    }
}

void FE2D::ResourceManager::load_available_metadata() {
    auto files = scan_folder(FOR_PATH.get_assets_path(), ScanMode::FILES);
    for (const auto& path : files) {
        try {
            if (path.extension() != L".fs")
                continue;

            std::ifstream file(path);
            if (!file.good())
                FOR_RUNTIME_ERROR("Failed to open .fs");

            json j;
            file >> j;
            file.close();

            std::filesystem::path res_path = path;
            res_path.replace_extension(); // clear .fs extension

            std::filesystem::path ext = ResourceLoader::get_extension(res_path);

            const size_t res_index = j["Resource Index"];
            const size_t hash_code = m_ResourceLoader.get_hash_code(ext);

            if (!hash_code)
                continue;

            m_ResourceCache.set_metadata(path, hash_code, res_index);
        }
        catch (const std::exception& e) {
            SAY("WARNING : Failed to load metadata\nPath : " << path << "\nException : " << e.what());
        }
    }
}

void FE2D::ResourceManager::SaveResources() {
    for (const auto& resource_array : m_ResourceCache.get_resource_array()) {
        for (const auto& resource : resource_array.second) {
            
            IResource* res = resource.second;
            const size_t res_hash_code = typeid(*res).hash_code();
            const size_t res_index = resource.first;

            const auto& metadata = m_ResourceCache.get_metadata(res_hash_code, res_index);

            if (!metadata.has_value()) {
                SAY("WARNING : Failed to serialize a resource\nThere is no metadata");
                continue;
            }

            std::ofstream file(metadata.value());
            if (!file.good()) {
                SAY("WARNING : Failed to serialize a resource\nFailed to open the file");
                continue;
            }

            json j;
            j["Resource Index"] = res_index;
            j["Resource Data"]  = res->Serialize();

            file << j;

            file.close();
        }
    }
}

std::pair<size_t, size_t> FE2D::ResourceManager::getInfoByPath(const std::filesystem::path& path) const {
    auto& metadata_array = m_ResourceCache.get_metadata_array();

    static thread_local std::unordered_map<std::filesystem::path, std::pair<size_t, size_t>> localCache;

    auto it = localCache.find(path);
    if (it != localCache.end()) {
        return it->second;
    }

    for (auto& metadata_type_array : metadata_array) {
        for (auto& metadata : metadata_type_array.second) {

            size_t hash_code = metadata_type_array.first;
            size_t index = metadata.first;
            std::filesystem::path res_path = metadata.second;
            res_path.replace_extension(); // clear .fs extension
            
            if (res_path == path) {
                std::pair<size_t, size_t> pair = { hash_code, index };

                localCache.emplace(path, pair);
                return pair;
            }
        }
    }

    return {};
}

IResource* FE2D::ResourceManager::getResource(size_t hash_code, size_t index) {
    // resource index | resource pointer
    static thread_local std::unordered_map<size_t, IResource*> localCache;

    auto it = localCache.find(index);
    if (it != localCache.end()) {
        return it->second;
    }

    // here you can use static_cast because T is definitely derived from Resource
    IResource* resource = m_ResourceCache.get_resource(hash_code, index);

    if (resource) {
        localCache.emplace(index, resource);
        return resource;
    }

    const auto& metadata = m_ResourceCache.get_metadata(hash_code, index);
    if (!metadata.has_value())
        return this->fallbackResource(hash_code);

    try {
        std::filesystem::path res_path = metadata.value();
        res_path.replace_extension(); // clear .fs extension
        this->load_resource(res_path);

        // here you can use static_cast because T is definitely derived from Resource
        if (resource = m_ResourceCache.get_resource(hash_code, index)) {
            localCache.emplace(index, resource);
            return resource;
        }

        SAY("ERROR : Failed to get a resource. Resource still missing after loading attempt");
        return this->fallbackResource(hash_code);
    }
    catch (const std::exception& e) {
        SAY("ERROR : Failed to get a resource. Using the first loaded\nException : " << e.what());
        return this->fallbackResource(hash_code);
    }
}

IResource* FE2D::ResourceManager::fallbackResource(size_t hash_code) {
    auto& stored = m_ResourceCache.get_resource_array();

    auto it = stored.find(hash_code);
    if (it == stored.end() || it->second.empty())
        FOR_RUNTIME_ERROR("Failed to fallback\nNo loaded resources");
    // here you can use static_cast because T is definitely derived from Resource
    return it->second.begin()->second;
}

void FE2D::ResourceManager::load_resource(const std::filesystem::path& file_path) {
    if (!std::filesystem::is_regular_file(file_path)) {
        SAY("WARNING : Failed to load a resource\nIt's not a file or doesn't exist\nPath : " << file_path);
        return;
    }

    if (ResourceLoader::texture_supported_extensions.contains(file_path.extension())) {
        this->handle_loading<Texture>(file_path);
    }

    if (ResourceLoader::animation_supported_extensions.contains(file_path.extension())) {
        this->handle_loading<Animation>(file_path);
    }

    if (ResourceLoader::audio_supported_extensions.contains(file_path.extension())) {
        // TODO : Make audio system

    }

    if (ResourceLoader::font_supported_extensions.contains(file_path.extension())) {
        // TODO : Make text rendering

    }
}