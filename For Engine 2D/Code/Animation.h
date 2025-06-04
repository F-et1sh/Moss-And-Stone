#pragma once  
#include "IResource.h"  
#include "ResourceManager.h"

namespace FE2D {  
	class FOR_API Animation : public IResource {  
	public:  
		FOR_CLASS_DEFAULT(Animation)  

		void Release()override;  
		bool LoadFromFile(const std::filesystem::path& file_path)override;  

		inline float getMax()const noexcept { return 6; } // TODO..
		
		vec4 getFrame(float time)const noexcept;
		ResourceID<Texture> getTexture(ResourceManager& resource_manager);

	private:  
		std::vector<vec4, boost::fast_pool_allocator<vec4>> m_Frames;
		ResourceID<Texture> m_TextureID;

		std::filesystem::path m_TexturePath;
	};
}