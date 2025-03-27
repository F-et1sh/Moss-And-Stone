#include "forpch.h"
#include "ResourceLoader.h"

#include "Texture.h"

std::wstring FE2D::ResourceLoader::get_extension(const std::filesystem::path& file_path) {
    if (!file_path.has_extension())
        return std::wstring();

    std::wstring ext = file_path.extension().wstring();
    std::transform(ext.begin(), ext.end(), ext.begin(), std::towlower); // ensure case-insensitivity

    return ext;
}

const size_t FE2D::ResourceLoader::get_hash_code(const std::filesystem::path& file_path) {
    if (ResourceLoader::texture_supported_extensions.contains(file_path.extension())) {
        return typeid(Texture).hash_code();
    }

    if (ResourceLoader::audio_supported_extensions.contains(file_path.extension())) {
        //return typeid("AudioBuffer")
    }

    if (ResourceLoader::font_supported_extensions.contains(file_path.extension())) {
        // TODO : Make text rendering

    }

    return 0;
}
