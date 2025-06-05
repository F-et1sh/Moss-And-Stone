#include "forpch.h"
#include "Animation.h"

void FE2D::Animation::Release() {
    m_Frames.clear();
    m_Duration = 0.0f;
    m_TextureID = ResourceID<Texture>(0);
    m_TexturePath.clear();
}

bool FE2D::Animation::LoadFromFile(const std::filesystem::path& file_path) {
    this->Release();

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
        Frame frame;

        json j_frame = frame_data["frame"];
        frame.uv.x = j_frame["x"].get<int>();
        frame.uv.y = j_frame["y"].get<int>();
        frame.uv.z = j_frame["w"].get<int>();
        frame.uv.w = j_frame["h"].get<int>();

        // convert milliseconds to seconds
        frame.duration = static_cast<float>(frame_data["duration"].get<int>()) / 1000.0f;
        m_Duration += frame.duration;

        m_Frames.emplace_back(frame);
    }

    return true;
}

vec4 FE2D::Animation::getFrameUV(float time) const noexcept {
    if (m_Frames.empty())
        return vec4();

    float time_mod = std::fmod(time, m_Duration);

    float accumulated_time = 0.0f;
    for (const auto& frame : m_Frames) {
        accumulated_time += frame.duration;
        if (time_mod < accumulated_time)
            return frame.uv;
    }

    return m_Frames.back().uv;
}

ResourceID<Texture> FE2D::Animation::getTexture(ResourceManager& resource_manager) {
    if (m_TextureID.uuid == FE2D::UUID(0)) {
        auto uuid = resource_manager.GetResourceByPath(m_TexturePath);
        m_TextureID = ResourceID<Texture>(uuid);
    }
    return m_TextureID;
}
