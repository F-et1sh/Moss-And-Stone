#pragma once
#include "IResource.h"
#include "ResourceManager.h"

namespace FE2D {
	class FOR_API Animation : public IResource {
    public:
        using frame = size_t;
        using texture = size_t;
    public:
		Animation() = default;
		~Animation() = default;

		void Release()override;
		bool LoadFromFile(const std::filesystem::path& file_path)override;

        void OnEditorDraw(IMGUI& imgui)override;

		json Serialize()const override;
		void Deserialize(const json& j)override;

    public:
        texture getFrameTexture(frame frameIndex)const;
        size_t getFrameCount()const noexcept{ return m_Frames.size(); }

    private:
        std::vector<texture> m_Frames;
        bool m_IsLooped = true;
	};
}