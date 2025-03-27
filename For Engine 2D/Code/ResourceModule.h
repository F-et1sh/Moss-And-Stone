#pragma once

namespace FE2D {
	class FOR_API ResourceManager;

	// ResourceModule - it's a class to inherit for modules of ResourceManager
	// Modules : ResourceSystem, ResourceLoader, ResourceCache, ResourceUI
	template<typename ModuleType>
	class FOR_API ResourceModule {
	public:
		ResourceModule() = default;
		~ResourceModule() = default;
	protected:
		ResourceManager* m_ResourceManager = nullptr;
	protected:
		inline void setResourceManager(ResourceManager* load_resourceManager)noexcept {
			if (!load_resourceManager) {
				SAY("ERROR : Failed to set ResourceManager in " << typeid(ModuleType).name());
				return;
			}
			m_ResourceManager = load_resourceManager;
		}
	};
}