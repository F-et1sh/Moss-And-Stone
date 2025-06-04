#include "forpch.h"
#include "Animation.h"

void FE2D::Animation::Release() {
    m_Frames.clear();
    m_TextureID = ResourceID<Texture>(0);
    m_TexturePath.clear();
}

bool FE2D::Animation::LoadFromFile(const std::filesystem::path& file_path) {
    this->Release();

    // TODO : Duration

    std::ifstream file(file_path);
    if (!file.good()) {
        SAY("ERROR : Failed to open file");
        return false;
    }

    json j;
    file >> j;

    if (j.contains("meta") && j["meta"].contains("image")) {
        m_TexturePath = file_path.parent_path() / j["meta"]["image"].get<std::string>();
    }
    else {
        SAY("ERROR : Broken file\nThere is no \"meta\" or \"meta/image\"");
        return false;
    }

    if (!j.contains("frames") || !j["frames"].is_object()) {
        SAY("ERROR : Broken file\nThere is no \"frames\" or \"frames\" is not object");
        return false;
    }

    for (auto& [frame_name, frame_data] : j["frames"].items()) {
        vec4 frame = vec4();

        json j_frame = frame_data["frame"];
        frame.x = j_frame["x"].get<int>();
        frame.y = j_frame["y"].get<int>();
        frame.z = j_frame["w"].get<int>();
        frame.w = j_frame["h"].get<int>();

        m_Frames.emplace_back(frame);
    }

    return true;
}

vec4 FE2D::Animation::getFrame(float time) const noexcept {
    if (m_Frames.empty())
        return vec4();

    constexpr float dr = 0.1f;
    size_t index = static_cast<int>(time / dr) % m_Frames.size();
    return m_Frames[index];
}

ResourceID<Texture> FE2D::Animation::getTexture(ResourceManager& resource_manager) {
    if (m_TextureID.uuid == FE2D::UUID(0)) {
        auto uuid = resource_manager.GetResourceByPath(m_TexturePath);
        m_TextureID = ResourceID<Texture>(uuid);
        return ResourceID<Texture>(uuid);
    }
    else return m_TextureID;
}
