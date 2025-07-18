#pragma once

namespace FE2D {
	/* forward declarations */
	class Texture;
	class Animation;
	class Prefab;

	// .fs		 - For Save � metadata for resources				   
	// .forscene - For Scene � scene files						   
	// .fa		 - For Animation � Aseprite animation				   
	// .fp		 - For Prefab � prefab files						   
	// .fpv		 - For Project Variables - contans project variables 

	class FOR_API ResourceLoader : public IResourceModule {
	public:
		ResourceLoader(ResourceManager& resource_manager) {
			setResourceManager(resource_manager);
		}
		~ResourceLoader() = default;

		static bool is_supported(const std::filesystem::path& ext);
		
		void LoadResource(const std::filesystem::path& full_path);
		void LoadMetadata(const std::filesystem::path& full_path);
		void LoadFallback(const std::filesystem::path& filename);

		void CreateMetadata(const std::filesystem::path& full_path, FE2D::UUID uuid);

		template<typename T, typename... Args> requires std::is_base_of_v<IResource, T>
		void CreateResource(const std::filesystem::path& full_path, Args&&... args) {
			IResource* resource = new T(std::forward<Args>(args)...);
			FE2D::UUID uuid = FE2D::UUID(); // generate new UUID

			resource->UploadToFile(full_path);
			std::filesystem::path metadata_full_path = full_path.wstring() + L".fs";
			this->CreateMetadata(metadata_full_path, uuid);
			this->LoadMetadata(metadata_full_path);

			std::filesystem::path resource_relative_path = std::filesystem::relative(full_path, FOR_PATH.get_resources_path());
			this->cache_resource(uuid, resource, resource_relative_path);
		}

	private:
		template<typename T> requires std::is_base_of_v<IResource, T>
		IResource* load_fallback(const std::filesystem::path& filename) {
			IResource* resource = new T();
			auto path = FOR_PATH.get_fallbacks_path() / filename;
			if (!resource->LoadFromFile(path)) {
				delete resource;
				resource = nullptr;
				FOR_RUNTIME_ERROR(("Failed to create fallback for " + std::string(typeid(T).name()) + "\nPath : " + path.string()));
			}
			
			this->fallback_resource(typeid(T).hash_code(), resource);

			return resource;
		}

		template<typename T> requires std::is_base_of_v<IResource, T>
		IResource* load_resource(const std::filesystem::path& full_path) {
			IResource* resource = new T();
			if (!resource->LoadFromFile(full_path)) {
				delete resource;
				SAY("ERROR : Failed to load a resource\nPath : " << full_path.c_str());
				return nullptr;
			}

			FE2D::UUID uuid = FE2D::UUID(0);

			std::filesystem::path metadata_full_path = full_path.wstring() + L".fs";

			std::ifstream file(metadata_full_path);
			if (file.good()) {
				json j;
				file >> j;

				if (j.contains("DATA") && j.contains("UUID")) {
					if (resource)
						resource->Deserialize(j["DATA"]);
					uuid = FE2D::UUID(j["UUID"].get<std::string>());
				}
				else {
					SAY("WARNING : There is no DATA or UUID in metadata\nPath : " << metadata_full_path.c_str());
					uuid = FE2D::UUID(); // generate new UUID
				}
			}
			else uuid = FE2D::UUID(); // generate new UUID

			auto metadata_path = std::filesystem::relative(metadata_full_path, FOR_PATH.get_assets_path());

			resource->setUUID(uuid);
			this->cache_resource(uuid, resource, metadata_path);

			return resource;
		}

		void cache_resource(FE2D::UUID uuid, IResource* resource, const std::filesystem::path& relative_path);
		void fallback_resource(size_t hash_code, IResource* resource);

	public:
		inline static const std::unordered_set<std::wstring> texture_supported_extensions = {
			L".png", L".jpg", L".bmp", L".tga", L".gif", L".hdr", L".pic", L".psd"
		};
		inline static const std::unordered_set<std::wstring> animation_supported_extensions = {
			L".fa",
		};
		inline static const std::unordered_set<std::wstring> prefab_supported_extensions = {
			L".fp",
		};
		inline static const std::unordered_set<std::wstring> audio_supported_extensions = {
			 L".wav", L".aif", L".aiff", L".au", L".snd", L".raw", L".pcm", L".sf", L".paf", L".svx",
			 L".8svx", L".nist", L".sph", L".voc", L".w64", L".flac", L".ogg", L".oga", L".opus",
			 L".mp3", L".mpc", L".m4a", L".aac", L".gsm", L".alac", L".caf", L".sd2", L".ircam", L".mat"
		};
		inline static const std::unordered_set<std::wstring> font_supported_extensions = {
			L".ttf", L".otf", L".ttc", L".woff", L".woff2", L".pfb", L".pfa"
		};
	};
}