#include "forpch.h"
#include "SoundDevice.h"

// Sound Device & Sound Context
ALCdevice* p_ALCDevice = nullptr;
ALCcontext* p_ALCContext = nullptr;

// Destruct FOR_APIor
SoundDevice::~SoundDevice() {
	// Set Sound Context to nullptr
	if (!alcMakeContextCurrent(nullptr)) SAY("ERROR : Failed to Set Sound Context to nullptr");
	// Destroy Sound Context
	alcDestroyContext(p_ALCContext);
	// Unset Sound Context
	if (p_ALCContext) SAY("ERROR : Failed to unset Sound Context During Close");
	// Close Sound Device
	if (!alcCloseDevice(p_ALCDevice)) SAY("ERROR : Failed to Close sound Device");
}

// Construct FOR_APIor
SoundDevice::SoundDevice() {
	// Set Default Sound Device
	p_ALCDevice = alcOpenDevice(nullptr); // He Said that nullptr is Default Device
	if (!p_ALCDevice) SAY("ERROR : Failed to get Sound Device");

	// Create Default Sound Context
	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);
	if (!p_ALCContext) SAY("ERROR : Failed to set Sound Context");

	// Make this Context Current
	if (!alcMakeContextCurrent(p_ALCContext)) SAY("ERROR : Failed to make Sound Context Current");

	// Set Name
	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT")) name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR) name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
}