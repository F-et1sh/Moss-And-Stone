#include "forpch.h"
#include "SoundManager.h"

#include <sndfile.h>
#include <inttypes.h>

// Stop all of Sounds, Delete they, Free all of Sound Buffers
void SoundManager::Release() {
	for (auto& sound : m_Sounds) {
		sound->stop();
		sound->release();
		sound = nullptr;
	}
	alDeleteBuffers(static_cast<ALsizei>(m_SoundBuffers.size()), m_SoundBuffers.data());
	m_SoundBuffers.clear();
	m_SoundPaths.clear();
	m_Sounds.clear();
}

// Load a Sound from File
bool SoundManager::loadSound(Sound* load_sound, std::string load_path) {
	auto it = std::find(m_SoundPaths.begin(), m_SoundPaths.end(), load_path);
	if (it != m_SoundPaths.end()) {
		load_sound->initialize();
		load_sound->set_buffer(m_SoundBuffers[std::distance(m_SoundPaths.begin(), it)]);
		m_Sounds.push_back(load_sound);
		return true;
	}
	else {
		// SF Info
		SF_INFO sfinfo;
		// Open the Audio file and Check that it's usable
		SNDFILE* sndfile = sf_open(load_path.c_str(), SFM_READ, &sfinfo);
		if (!sndfile) {
			SAY("ERROR : Failed to open Audio File - " << load_path.c_str());
			return false;
		}
		if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
			SAY("ERROR : Bad Sample count " << "(" << sfinfo.frames << ") in - " << load_path.c_str());
			sf_close(sndfile);
			return false;
		}

		// Get the Sound Format, and Figure out the OpenAL format
		ALenum format = AL_NONE;
		if (sfinfo.channels == 1) format = AL_FORMAT_MONO16;
		else if (sfinfo.channels == 2) format = AL_FORMAT_STEREO16;
		else if (sfinfo.channels == 3) {
			if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) format = AL_FORMAT_BFORMAT2D_16;
		}
		else if (sfinfo.channels == 4) {
			if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) format = AL_FORMAT_BFORMAT3D_16;
		}
		if (!format) {
			SAY("ERROR : Unsupported Channel count " << sfinfo.channels << "\nIn File - " << load_path.c_str());
			sf_close(sndfile);
			return false;
		}

		// Decode the Whole Audio file to a Buffer [ hah, like - "Whole World")) ]
		short* membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

		sf_count_t num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
		if (num_frames < 1) {
			free(membuf);
			sf_close(sndfile);
			SAY("ERROR : Failed to Read Samples in " << load_path.c_str());
			return false;
		}
		ALsizei num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

		// Create the Sound Buffer - data
		unsigned int buffer = 0;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

		free(membuf);
		sf_close(sndfile);

		// Check on Errors, and Clean up if Really So
		ALenum error = alGetError();
		if (error) {
			SAY("ERROR : OpenAL - " << alGetString(error));
			if (buffer && alIsBuffer(buffer)) alDeleteBuffers(1, &buffer);
			return false;
		}

		// Create Source at First
		load_sound->initialize();

		// Upload new Audio Data ( Buffer )
		load_sound->set_buffer(buffer);

		// Add new Path and Buffer
		m_SoundPaths.push_back(load_path.c_str());
		m_SoundBuffers.push_back(buffer);

		// Memory Sounds that was Loaded to Stop they in Release Function
		m_Sounds.push_back(load_sound);
	}
	return true;
}