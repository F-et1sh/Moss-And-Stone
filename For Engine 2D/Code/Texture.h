#pragma once
#include "IResource.h"
#include "ResourceManager.h"

namespace FE2D {
	class FOR_API Texture : public IResource {
	public:
		Texture() = default;
		~Texture() = default;

		void Release()override;
		bool LoadFromFile(const std::filesystem::path& file_path)override;

		void OnEditorDraw(IMGUI& imgui)override;

		inline void Bind()const noexcept;
		inline void Unbind()const noexcept;

		inline unsigned int reference() const noexcept { return m_Reference; }
		inline bool is_Initialized()const noexcept { return m_Reference > 0; }

	public:
		inline const void*			getData    ()const noexcept { return m_Data    ; }
		inline const bool			getIsLinear()const noexcept { return m_IsLinear; }
		inline const vec2&			getSize    ()const noexcept { return m_Size    ; }
		inline const unsigned int	getChannels()const noexcept { return m_Channels; }

		inline void				    setIsLinear(bool linear)noexcept;
	public:
		json Serialize()const override;
		void Deserialize(const json& j)override;
	private:
		void*			   m_Data     = nullptr;
		bool			   m_IsLinear = false;
		vec2			   m_Size     = vec2();
		unsigned int	   m_Channels = 0;
	private:			   
		unsigned int	   m_Reference = 0;
	};
}