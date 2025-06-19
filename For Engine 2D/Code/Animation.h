#pragma once  
#include "IResource.h"  
#include "ResourceManager.h"

namespace FE2D {  
	class FOR_API Animation : public IResource {
	public:
		struct FOR_API Frame {
			vec4 uv = vec4();
			float duration = 0.0f;

			Frame() = default;
			~Frame() = default;
		};
	public:  
		Animation() = default;
		~Animation() = default;

		bool LoadFromFile(const std::filesystem::path& file_path)override;

		inline float getDuration()const noexcept { return m_Duration; }
		
		vec4 getFrameUV(float time)const noexcept;
		ResourceID<Texture> getTexture(ResourceManager& resource_manager);

	private:
		std::vector<Frame, boost::fast_pool_allocator<Frame>> m_Frames;
		float m_Duration = 0.0f;
		
		ResourceID<Texture> m_TextureID;
		std::filesystem::path m_TexturePath;
	};
}