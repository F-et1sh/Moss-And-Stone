#pragma once

namespace FE2D {
	class FOR_API ResourceCache : public IResourceModule {
	public:
		ResourceCache(ResourceManager& resource_manager) { setResourceManager(resource_manager); }
		~ResourceCache() {
			this->clear_resources();
			this->clear_metadata();
			this->clear_fallbacks();
		}

		FOR_CLASS_NONCOPYABLE(ResourceCache)

		// pointer of resource
		using ResourcePointer = IResource*;
		// path to the .fs file of the resource
		// starts from assets folder - FOR_PATH.get_assets_path()
		using MetadataPath = std::filesystem::path;

		inline const auto& get_resource_array()const noexcept { return m_StoredResources; }
		inline const auto& get_metadata_array()const noexcept { return m_StoredMetadata ; }
		inline const auto& get_fallback_array()const noexcept { return m_StoredFallbacks; }

		void set_resource(FE2D::UUID uuid, ResourcePointer resource);
		void set_metadata(FE2D::UUID uuid, const MetadataPath& metadata);
		void set_fallback(size_t hash_code, ResourcePointer resource);

		ResourcePointer get_resource(FE2D::UUID uuid);
		MetadataPath get_metadata(FE2D::UUID uuid);
		ResourcePointer get_fallback(size_t hash_code);

		void remove_resource(FE2D::UUID uuid);
		void remove_metadata(FE2D::UUID uuid)noexcept;
		void remove_fallback(size_t hash_code);

		void clear_resources();
		void clear_metadata()noexcept;
		void clear_fallbacks();

	private:
		std::unordered_map<FE2D::UUID, ResourcePointer> m_StoredResources;
		std::unordered_map<FE2D::UUID, MetadataPath> m_StoredMetadata;

		// typeid(T).hash_code()
		std::unordered_map<size_t, ResourcePointer> m_StoredFallbacks;
	};
}