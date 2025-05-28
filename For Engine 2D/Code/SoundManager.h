#pragma once
#include "Sound.h"

namespace FE2D {
	class FOR_API SoundManager {
	public:
		// Default Construct FOR_APIor & Destruct FOR_APIor
		SoundManager() = default;
		~SoundManager() = default;

		// Stop all of Sounds, Delete they, Free all of Sound Buffers
		void Release();

		// Load a Sound from File
		bool loadSound(Sound* load_sound, std::string load_path);

	public:
		// Set Capacity for Sounds to Load
		inline void Reserve_SoundBuffers(unsigned int new_capacity) { m_SoundPaths.reserve(new_capacity); m_SoundBuffers.reserve(new_capacity); }

		// Set Capacity for Sounds Count
		inline void Reserve_Sounds(unsigned int new_capacity) { m_Sounds.reserve(new_capacity); }

	private:
		std::vector<std::string> m_SoundPaths;
		std::vector<unsigned int> m_SoundBuffers;
		std::vector<Sound*> m_Sounds;
	};
}