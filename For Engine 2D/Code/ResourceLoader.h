#pragma once
#include "IResource.h"

namespace FE2D {
	// ResourceLoader - it's a module of ResourceManager
	// It can serialize/deserialize .fs ( .fs - ForSave ) files and load resources from source
	class FOR_API ResourceLoader : public ResourceModule<ResourceLoader> {
	public:
		ResourceLoader(ResourceManager* load_resourceManager) {
			setResourceManager(load_resourceManager);
		}
		~ResourceLoader() = default;
	public:
		static std::filesystem::path get_extension(const std::filesystem::path& file_path);
		static size_t get_hash_code(const std::filesystem::path& ext);
		static bool is_supported(const std::filesystem::path& ext);

	public:
		static std::filesystem::path generate_unique_filename(const std::filesystem::path& file_path);
	public:
		template<typename T>
		static void CreateResource(const std::filesystem::path& file_path) {
			FOR_ASSERT((std::is_base_of_v<IResource, T>), "Failed to create a resource\nT must be devired by Resource");

			if (std::filesystem::is_directory(file_path))
				return;

			std::filesystem::path path = ResourceLoader::generate_unique_filename(file_path);
			
			std::ofstream file(path);
			if (!file.good()) {
				SAY("ERROR : Failed to create a resource\nPath : " << path);
				return;
			}

			IResource* resource = new T();
			file << resource->Serialize();

			file.close();
		}
	public:
		// returns std::pair with pointer of your resource - T*
		// and size_t - index of the resource
		template<typename T>
		static std::pair<T*, size_t> load_resource(const std::filesystem::path& file_path) {
			static_assert(std::is_base_of<IResource, T>::value, "ERROR : T must be derived from Resource");

			T* resource = new T();
			if (!resource->LoadFromFile(file_path))
				return { nullptr, 0 };

			size_t resource_index = 0;

			std::ifstream file(file_path.wstring() + L".fs");
			if (!file.good()) {
				resource_index = std::hash<std::filesystem::path>{}(file_path) ^ std::hash<const char*>{}(typeid(T).name());
			}
			else {
				json j;

				file >> j;
				file.close();

				resource_index		= j["Resource Index"];
				resource->Deserialize(j["Resource Data"]);
			}

			return { resource, resource_index };
		}

	public:
		inline static const std::unordered_set<std::wstring> texture_supported_extensions = {
			L".png", L".jpg", L".bmp", L".tga", L".gif", L".hdr", L".pic", L".psd"
		};
		inline static const std::unordered_set<std::wstring> animation_supported_extensions = {
			L".fa",
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