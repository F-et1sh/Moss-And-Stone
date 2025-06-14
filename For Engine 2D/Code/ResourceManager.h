#pragma once
#include "PathManager.h"
#include "UUID.h"

#include "IResourceModule.h"
#include "IResource.h"

#include "ResourceLoader.h"
#include "ResourceCache.h"

#include "ResourceID.h"

namespace FE2D {
	// ResourceManager - is a conductor-class, which manages all of its modules
	// Modules of ResourceManager : 
	// ResourceLoader - can serialize/deserialize .fs ( .fs - ForSave ) files and load/create resources
	// ResourceCache  - contains all of resources in cache, being a 'stupid' container
	// 
	// GLOBAL RULE OF MEMORY MANAGEMENT :
	// Resources are created in ResourceLoader with 'new' and deleted in ResourceCache with 'delete'
	class FOR_API ResourceManager {
	public:
		ResourceManager() = default;
		~ResourceManager() = default; // ñall Release() youself !!

		static const std::vector<unsigned char> load_file_to_memory(const std::wstring& file_path);

		enum class ScanMode { FILES, FOLDERS, BOTH };

		static std::vector<std::filesystem::path> scan_folder(
			const std::filesystem::path& folder_path,
			ScanMode mode,
			size_t start_capacity = 32);

		void Release();
		void Initialize();

		// this function scanning all of resource files from assets folder ( FOR_PATH.get_assets_path() )
		// and uploading them to cache. If they have no metadata this will generate it
		void load_available_resources();

		// this function scanning all of .fs files from resources folder and uploading all of metadata to cache
		// If not loaded resource will be called in ResourceManager::GetResource() function it will be loaded by info from its metadata
		void load_available_metadata();

		// this function scanning all resources in fallback folder
		void load_fallback_resources();

		// generate .fs files for resources
		void save_resources();

		template<typename T> requires std::is_base_of_v<IResource, T>
		inline T& GetResource(ResourceID<T> id) {
			static thread_local std::unordered_map<FE2D::UUID, IResource*> local_cache;

			auto it = local_cache.find(id.uuid);
			if (it != local_cache.end()) return *static_cast<T*>(it->second);

			T* resource = static_cast<T*>(m_ResourceCache.get_resource(id.uuid));

			if (resource) {
				local_cache.emplace(id.uuid, resource);
				return *resource;
			}

			auto metadata = m_ResourceCache.get_metadata(id.uuid);
			if (metadata.empty()) return this->GetFallbackResource<T>();

			std::filesystem::path resource_path = FOR_PATH.get_assets_path() / metadata.wstring();
			resource_path.replace_extension(); // clear .fs extension
			this->m_ResourceLoader.LoadResource(resource_path);

			resource = static_cast<T*>(m_ResourceCache.get_resource(id.uuid));
			if (resource) {
				local_cache.emplace(id.uuid, resource);
				return *resource;
			}

			SAY("ERROR : Failed to get a resource. Resource still missing after loading attempt");

			if (this->GetResourceByPath(resource_path) != FE2D::UUID(0))
				FOR_RUNTIME_ERROR(("Found broken .fs file. Please, fix or delete it\nPath : " + resource_path.string() + ".fs"));

			return this->GetFallbackResource<T>();
		}

		// this function might be slow. Be careful
		FE2D::UUID GetResourceByPath(const std::filesystem::path& path)const;

		template<typename T> requires std::is_base_of_v<IResource, T>
		inline void RemoveResource(ResourceID<T> id) { m_ResourceCache.remove_resource(id.uuid); }

	private:
		template<typename T> requires std::is_base_of_v<IResource, T>
		inline T& GetFallbackResource() {
			static size_t hash_code = typeid(T).hash_code();
			auto fallback = m_ResourceCache.get_fallback(hash_code);
			if (fallback) return *static_cast<T*>(fallback);
			FOR_RUNTIME_ERROR(("Failed to get a fallback of resource\nType : " + std::string(typeid(T).name())));
		}
	public:
		ResourceLoader& getLoader()noexcept { return m_ResourceLoader; }
		ResourceCache& getCache()noexcept { return m_ResourceCache; }

	private:
		ResourceLoader m_ResourceLoader = ResourceLoader(*this);
		ResourceCache  m_ResourceCache  = ResourceCache (*this);
	};
}