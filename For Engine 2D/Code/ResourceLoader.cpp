#include "forpch.h"
#include "ResourceLoader.h"

bool FE2D::ResourceLoader::is_supported(const std::filesystem::path& ext) {
    if (ResourceLoader::texture_supported_extensions  .contains(ext)) return true;
    if (ResourceLoader::animation_supported_extensions.contains(ext)) return true;
    //if (ResourceLoader::audio_supported_extensions    .contains(ext)) return true; not done
    //if (ResourceLoader::font_supported_extensions     .contains(ext)) return true; not done

    return false;
}

void FE2D::ResourceLoader::LoadResource(const std::filesystem::path& full_path) {
    if (!is_supported(full_path.extension()))
        return;

    if (texture_supported_extensions  .contains(full_path.extension())) load_resource<Texture  >(full_path);
    if (animation_supported_extensions.contains(full_path.extension())) load_resource<Animation>(full_path);
}

void FE2D::ResourceLoader::LoadMetadata(const std::filesystem::path& full_path) {
    auto resource_path = full_path;
    resource_path.replace_extension();

    if (!std::filesystem::exists(resource_path))
        return;

    FE2D::UUID uuid = FE2D::UUID(0);

    std::ifstream file(full_path);
    if (!file.good())
        SAY("WARNING : Failed to open metadata file\nPath : " << full_path);
    else {
        json j;
        file >> j;

        if (j.contains("UUID")) {
            uuid = FE2D::UUID(j["UUID"].get<std::string>());
        }
        else {
            SAY("WARNING : There is no UUID in metadata\nPath : " << full_path);
            uuid = FE2D::UUID(); // generate new UUID
        }
    }

    auto metadata_path = std::filesystem::relative(full_path, FOR_PATH.get_assets_path());
    m_ResourceManager->getCache().set_metadata(uuid, metadata_path);
}

void FE2D::ResourceLoader::LoadFallback(const std::filesystem::path& filename) {
    if (!is_supported(filename.extension()))
        return;

    if (texture_supported_extensions  .contains(filename.extension())) load_fallback<Texture  >(filename);
    if (animation_supported_extensions.contains(filename.extension())) load_fallback<Animation>(filename);
}

void FE2D::ResourceLoader::CreateResource(const std::filesystem::path& full_path) {
    if (!is_supported(full_path.extension()))
        return;

    if (texture_supported_extensions  .contains(full_path.extension())) create_resource<Texture  >(full_path);
    if (animation_supported_extensions.contains(full_path.extension())) create_resource<Animation>(full_path);
}

void FE2D::ResourceLoader::CreateMetadata(const std::filesystem::path& full_path, FE2D::UUID uuid) {
    std::ofstream file(full_path);
    if (!file.good()) {
        SAY("WARNING : Failed to create metadata" << "\nUUID : " << uuid.ToString().c_str() << "\nPath : " << full_path);
        return;
    }
    json j;

    if (auto resource = m_ResourceManager->getCache().get_resource(uuid))
        j["DATA"] = resource->Serialize();

    j["UUID"] = uuid.ToString();

    file << j;
}

void FE2D::ResourceLoader::cache_resource(FE2D::UUID uuid, IResource* resource, const std::filesystem::path& relative_path) {
    if (uuid == FE2D::UUID(0))  return;
    if (resource)               m_ResourceManager->getCache().set_resource(uuid, resource);
    if (!relative_path.empty()) m_ResourceManager->getCache().set_metadata(uuid, relative_path);
}

void FE2D::ResourceLoader::fallback_resource(size_t hash_code, IResource* resource) {
    if (resource)               m_ResourceManager->getCache().set_fallback(hash_code, resource);
}
