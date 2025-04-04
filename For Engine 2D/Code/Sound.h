#pragma once

namespace FE2D {
	struct FOR_API Sound {
	public:
		inline void initialize();
		inline void release();

		inline unsigned int get_source() { return s_source; }
		inline void         set_source(unsigned int source) { s_source = source; }

		inline int  get_buffer();
		inline void set_buffer(int buffer);

		inline void play();
		inline void pause();
		inline void stop();

		inline int get_state();

		inline int  get_loop();
		inline void set_loop(int isLooping);

		inline vec3 get_Position();
		inline void set_Position(vec3 load_pos);

		inline float get_pitch();
		inline void  set_pitch(float load_pitch);

		inline float get_volume();
		inline void  set_volume(float load_volume);

		Sound() = default;
		~Sound() = default;

	private:
		unsigned int s_source = 0;
	public:
		enum State {
			INITIAL,
			PLAYING,
			PAUSED,
		};
	};
}