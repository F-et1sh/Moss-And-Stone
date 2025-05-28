#pragma once

namespace FE2D {
	class FOR_API ResourceManager;

	class FOR_API IResourceModule {
	public:
		IResourceModule() = default;
		virtual ~IResourceModule() = default;

	protected:
		ResourceManager* m_ResourceManager = nullptr;

		inline void setResourceManager(ResourceManager& resource_manager)noexcept { m_ResourceManager = &resource_manager; }
	};
}