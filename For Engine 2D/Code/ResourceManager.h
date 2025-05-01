#pragma once
#include "ResourceModule.h"
#include "IResource.h"

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
		inline std::wstring get_extension(const std::filesystem::path& file_path)const noexcept { return m_ResourceLoader.get_extension(file_path); }
	public:

	public:
		template<typename T>
		void removeResource(size_t index) {
			FOR_ASSERT((std::is_base_of_v<IResource, T>), "T must be derived from Resource");
			
			const size_t hash_code = typeid(T).hash_code();
			m_ResourceCache.remove_resource(hash_code, index);
		}

		void removeResource(size_t hash_code, size_t index) {
			m_ResourceCache.remove_resource(hash_code, index);
		}
	public:
		// this function might be slow. Be careful
		// std::pair<size_t, size_t> - are type (typeid(T).hash_code) and resource index
		std::pair<size_t, size_t> getInfoByPath(const std::filesystem::path& path)const;

	public:
		template<typename T>
		T& getResource(size_t index) {
			FOR_ASSERT((std::is_base_of_v<IResource, T>), "T must be derived from Resource");

			const size_t hash_code = typeid(T).hash_code();

			// resource index | resource pointer
			static thread_local std::unordered_map<size_t, IResource*> localCache;

			auto it = localCache.find(index);
			if (it != localCache.end()) {
				return *static_cast<T*>(it->second);
			}

			// here you can use static_cast because T is definitely derived from Resource
			T* resource = static_cast<T*>(m_ResourceCache.get_resource(hash_code, index));

			if (resource) {
				localCache.emplace(index, resource);
				return *resource;
			}

			const auto& metadata = m_ResourceCache.get_metadata(hash_code, index);
			if (!metadata.has_value())
				return this->fallbackResource<T>();

			try {
				std::filesystem::path res_path = metadata.value();
				res_path.replace_extension(); // clear .fs extension
				this->load_resource(res_path);

				// here you can use static_cast because T is definitely derived from Resource
				resource = static_cast<T*>(m_ResourceCache.get_resource(hash_code, index));
				if (resource) {
					localCache.emplace(index, resource);
					return *resource;
				}

				SAY("ERROR : Failed to get a resource. Resource still missing after loading attempt");
				return this->fallbackResource<T>();
			}
			catch (const std::exception& e) {
				SAY("ERROR : Failed to get a resource. Using the first loaded\nException : " << e.what());
				return this->fallbackResource<T>();
			}
		}

		IResource* getResource(size_t hash_code, size_t index);

	private:
		template<typename T>
		T& fallbackResource() {
			FOR_ASSERT((std::is_base_of_v<IResource, T>), "T must be derived from Resource");

			auto& stored = m_ResourceCache.get_resource_array();

			const size_t hash_code = typeid(T).hash_code();

			auto it = stored.find(hash_code);
			if (it == stored.end() || it->second.empty())
				FOR_RUNTIME_ERROR("Failed to fallback\nNo loaded resources of type " + std::string(typeid(T).name()));
			// here you can use static_cast because T is definitely derived from Resource
			return *static_cast<T*>(it->second.begin()->second);
		}

		IResource* fallbackResource(size_t hash_code);

	private:
		void load_resource(const std::filesystem::path& file_path);
		
		template<typename T>
		void handle_loading(const std::filesystem::path& file_path) {
			auto resource_info = m_ResourceLoader.load_resource<T>(file_path);

			IResource* resource = resource_info.first;
			const size_t hash_code = typeid(T).hash_code();
			const size_t res_index = resource_info.second;

			if (!resource) {
				SAY("Failed to Initialize a resource\nPath : " << file_path);
				return;
			}

			std::filesystem::path metadata_path = file_path.wstring() + L".fs";

			m_ResourceCache.set_resource(resource, hash_code, res_index);
			m_ResourceCache.set_metadata(metadata_path, hash_code, res_index);
		}
	public:
		inline void ClearCache() { m_ResourceCache.clear(); }

		ResourceLoader& getLoader()noexcept { return m_ResourceLoader; }
		ResourceCache& getCache()noexcept { return m_ResourceCache; }

	private:
		ResourceLoader m_ResourceLoader = ResourceLoader(this);
		ResourceCache  m_ResourceCache  = ResourceCache (this);
	};
}