#include "forpch.h"
#include "ResourceLoader.h"

#include "Texture.h"

std::filesystem::path FE2D::ResourceLoader::get_extension(const std::filesystem::path& file_path) {
    if (!file_path.has_extension())
        return std::wstring();

    std::wstring ext = file_path.extension().wstring();
    std::transform(ext.begin(), ext.end(), ext.begin(), std::towlower); // ensure case-insensitivity

    return ext;
}

size_t FE2D::ResourceLoader::get_hash_code(const std::filesystem::path& ext) {
    if (ResourceLoader::texture_supported_extensions  .contains(ext)) return typeid(Texture  ).hash_code();
    if (ResourceLoader::animation_supported_extensions.contains(ext)) return typeid(Animation).hash_code();
    if (ResourceLoader::audio_supported_extensions    .contains(ext)) /* TODO : Create Audio */;
    if (ResourceLoader::font_supported_extensions     .contains(ext)) /* TODO : Create Font  */;

    return 0;
}

bool FE2D::ResourceLoader::is_supported(const std::filesystem::path& ext) {
    if (ResourceLoader::texture_supported_extensions  .contains(ext)) return true;
    if (ResourceLoader::animation_supported_extensions.contains(ext)) return true;
    //if (ResourceLoader::audio_supported_extensions    .contains(ext)) return true; not done
    //if (ResourceLoader::font_supported_extensions     .contains(ext)) return true; not done

    return false;
}

std::filesystem::path FE2D::ResourceLoader::generate_unique_filename(const std::filesystem::path& file_path) {
    std::filesystem::path path = file_path;
    std::filesystem::path stem = path.stem();
    std::filesystem::path extension = path.extension();
    int count = 1;

    while (std::filesystem::exists(path)) {
        path.replace_filename(stem.wstring() + L" (" + std::to_wstring(count) + L")" + extension.wstring());
        count++;
    }

    return path;
}
