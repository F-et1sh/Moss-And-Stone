#pragma once
#include "IResource.h"
#include "ResourceManager.h"

namespace FE2D {
	class FOR_API Animation : public IResource {
	public:
		enum class Status {
			STOPPED,
			PAUSED,
			PLAYING,
		};
	public:
		Animation() = default;
		~Animation() = default;

		void Release()override;
		bool LoadFromFile(const std::filesystem::path& file_path)override;
		void CreateSource(const std::filesystem::path& file_path)const override;

        void OnEditorDraw(IMGUI& imgui)override;

		json Serialize()const override;
		void Deserialize(const json& j)override;

		inline void Play ()noexcept { m_Status = Status::PLAYING; }
		inline void Pause()noexcept { m_Status = Status::PAUSED ; }
		inline void Stop ()noexcept { m_Status = Status::STOPPED; }

		inline size_t getFrameCount()const noexcept { return m_Frames.size(); }
			   
		inline void setLooped(bool looped)noexcept { m_IsLooped = looped; }
		inline bool getLooped()const noexcept { return m_IsLooped; }

    private:
		std::vector<FE2D::UUID> m_Frames;
		bool m_IsLooped = false;

		Status m_Status = Status::STOPPED;
	};
}