#pragma once
#include "ResourceModule.h"
#include "Resource.h"

#include "ResourceLoader.h"
#include "ResourceCache.h"

#include "PathManager.h"

namespace FE2D {
	// ResourceManager - it's a class, which is contain and manage all of modules
	// Modules of ResourceManager : 
	// ResourceLoader   - can serialize/deserialize .fs ( .fs - ForSave ) files and load resources from source
	// ResourceCache  - contains all of resources
	class FOR_API ResourceManager {
	public:
		ResourceManager() = default;
		~ResourceManager() = default; // ñall Release() function youself
	public:
		static const std::vector<unsigned char> load_file_to_memory(const std::wstring& file_path);

	public:
		enum class ScanMode { FILES, FOLDERS, BOTH };

		static const std::vector<std::filesystem::path> scan_folder(
																	const std::filesystem::path& folder_path, 
																	ScanMode mode, 
																	size_t start_capacity = 32);
	public:
		void Release();
		void Initialize();

		// this function scanning all of resource files from resources folder
		// and uploading them to cache. If they have no metadata this will generate it
		void load_available_resources();

		// this function scanning all of .fs files from resources folder
		// and uploading all of metadata to cache
		// If not loaded resource will be called in ResourceManager::getResource<>() funciton
		// it will be loaded by info from its metadata
		void load_available_metadata();
	public:
		void SaveResources();
	public:
		inline std::wstring get_extension(const std::filesystem::path& file_path)const {
			return m_ResourceLoader.get_extension(file_path);
		}

		// this function never returns nullptr
		template<typename T>
		T& getResource(size_t index) {
			static_assert(std::is_base_of<Resource, T>::value, "ERROR : T must be derived from Resource");

			const size_t hash_code = typeid(T).hash_code();

			static thread_local std::unordered_map<size_t, T*> localCache;

			auto it = localCache.find(index);
			if (it != localCache.end()) {
				return *it->second;
			}

			// here you can use static_cast because T is definitely derived from Resource
			T* resource = static_cast<T*>(m_ResourceCache.get_resource(hash_code, index));
			
			if (resource) {
				localCache[hash_code] = resource;
				return *resource;
			}

			const auto& metadata = m_ResourceCache.get_metadata(hash_code, index);
			if (!metadata.has_value())
				return this->fallbackResource<T>();

			try {
				std::filesystem::path res_path = metadata.value();
				res_path.replace_extension(); // clear .fs extension
				this->load_resource(FOR_PATH.get_resources_path() / res_path);
				
				// here you can use static_cast because T is definitely derived from Resource
				resource = static_cast<T*>(m_ResourceCache.get_resource(hash_code, index));
				if (resource)
					return *resource;

				SAY("ERROR : Failed to get a resource. Resource still missing after loading attempt");
				return this->fallbackResource<T>();
			}
			catch (const std::exception& e) {
				SAY("ERROR : Failed to get a resource. Using the first loaded\nException : " << e.what());
				return this->fallbackResource<T>();
			}
		}

	public:
		void load_resource(const std::filesystem::path& file_path);

		template<typename T>
		T& fallbackResource() {
			static_assert(std::is_base_of<Resource, T>::value, "ERROR : T must be derived from Resource");
			
			auto& stored = m_ResourceCache.get_resource_array();

			const size_t hash_code = typeid(T).hash_code();

			auto it = stored.find(hash_code);
			if (it == stored.end() || it->second.empty())
				FOR_RUNTIME_ERROR("Failed to fallback\nNo loaded resources of type " + std::string(typeid(T).name()));
			// here you can use static_cast because T is definitely derived from Resource
			return *static_cast<T*>(it->second.begin()->second);
		}
	public:
		void ClearCache() { m_ResourceCache.clear(); }

	public:
		ResourceLoader m_ResourceLoader = ResourceLoader(this);
		ResourceCache  m_ResourceCache  = ResourceCache (this);
	};
}