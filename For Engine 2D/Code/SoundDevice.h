#pragma once
#include <fordebug.h>

namespace FE2D {
	class FOR_API SoundDevice {
	public:
		static SoundDevice* get() { static SoundDevice* snd_device = new SoundDevice(); return snd_device; }
	private:
		SoundDevice();
		~SoundDevice();
	};
}