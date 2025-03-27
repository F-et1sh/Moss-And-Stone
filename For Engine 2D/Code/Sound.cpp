#include "forpch.h"
#include "Sound.h"

#pragma region Initialize & Release
inline void Sound::initialize() { alGenSources(1, &s_source); this->set_volume(this->get_volume()); }
inline void Sound::release() { alDeleteSources(1, &s_source); }
#pragma endregion

#pragma region Buffer
inline int  Sound::get_buffer() { int buffer; alGetSourcei(s_source, AL_BUFFER, &buffer); return buffer; }
inline void Sound::set_buffer(int buffer) { alSourcei(s_source, AL_BUFFER, buffer); }
#pragma endregion

#pragma region Play, Pause, Stop
inline void Sound::play() { alSourcePlay(s_source); }
inline void Sound::pause() { alSourcePause(s_source); }
inline void Sound::stop() { alSourceStop(s_source); }
#pragma  endregion

#pragma region Get State
inline int Sound::get_state() {
	int state;
	alGetSourcei(s_source, AL_SOURCE_STATE, &state);
	switch (state) {
	case AL_INITIAL:return Sound::State::INITIAL; break;
	case AL_PLAYING:return Sound::State::PLAYING; break;
	case AL_PAUSED:return Sound::State::PAUSED; break;
	}
}
#pragma endregion

#pragma region Looping
inline int  Sound::get_loop() { int looping = 0; alGetSourcei(s_source, AL_LOOPING, &looping); return looping; }
inline void Sound::set_loop(int isLooping) { alSourcei(s_source, AL_LOOPING, isLooping); }
#pragma endregion

#pragma region Position
inline vec3 Sound::get_Position() { float pos_x = 0.0f; float pos_y = 0.0f; float pos_z = 0.0f; alGetSource3f(s_source, AL_POSITION, &pos_x, &pos_y, &pos_z); return vec3(pos_x, pos_y, pos_z); }
inline void Sound::set_Position(vec3 load_pos) { alSource3f(s_source, AL_POSITION, load_pos.x, load_pos.y, load_pos.z); }
#pragma endregion

#pragma region Pitch
inline float Sound::get_pitch() { float pitch; alGetSourcef(s_source, AL_PITCH, &pitch); return pitch; }
inline void  Sound::set_pitch(float load_pitch) { alSourcef(s_source, AL_PITCH, load_pitch); }
#pragma endregion

#pragma region Volume
inline float Sound::get_volume() { float volume; alGetSourcef(s_source, AL_GAIN, &volume); return volume; }
inline void  Sound::set_volume(float load_volume) { alSourcef(s_source, AL_GAIN, load_volume); }
#pragma endregion