#pragma once

namespace FE2D {
	struct FOR_API anim_key {
	public:
		vec4 texCoords = vec4();
		bool isFlipped = false;
		bool active = false;

		anim_key(vec4 texCoords, bool isFlipped) : texCoords(texCoords), isFlipped(isFlipped), active(true) {}

		anim_key() = default;
		~anim_key() = default;
	};

	struct FOR_API Animation {
	private:
		std::unordered_map<int, anim_key> m_Keys;
		anim_key m_LastKey;
	private:
		bool m_Repeat = true;
		float m_Speed = 10.0f;

		unsigned int m_FramesCount = 0;
	private:
		std::string m_TexturePath = FOR_DEFAULT_TEXTURE_PATH;
	public:
		inline const bool& get_repeated() const noexcept { return m_Repeat; }
		inline void set_repeated(bool isRepeated) noexcept { m_Repeat = isRepeated; }

		inline const float& get_speed() const noexcept { return m_Speed; }
		inline void set_speed(float load_speed) noexcept { m_Speed = load_speed; }

		inline const unsigned int& get_FramesCount() const noexcept { return m_FramesCount; }
		inline void set_FramesCount(unsigned int load_framesCount) noexcept { m_FramesCount = load_framesCount; }

		inline const std::string& get_TexturePath() const noexcept { return m_TexturePath; }
		inline void set_TexturePath(std::string load_texturePath) noexcept { m_TexturePath = load_texturePath; }

		inline void reserve_keys(unsigned int new_capacity) noexcept { m_Keys.reserve(new_capacity); }

		inline void add_key(unsigned int frame, vec4 key_texCoords, bool key_isFlipped) {
			m_Keys.insert(std::pair<int, anim_key>(frame, anim_key(key_texCoords, key_isFlipped)));
			m_FramesCount = max(m_FramesCount, frame);
			m_LastKey = anim_key(key_texCoords, key_isFlipped);
		}
		inline const anim_key& get_key(unsigned int frame) {
			if (m_Keys[frame].active) {
				m_LastKey = m_Keys[frame];
				return m_Keys[frame];
			}
			else return m_LastKey;
		}

		inline void make_anim_auto(unsigned int frames_count, vec4 texCoords, bool is_flipped, std::string texPath) {
			this->reserve_keys(frames_count);
			for (unsigned int i = 0; i < frames_count; i++)
				this->add_key(i, vec4(texCoords.x + texCoords.z * i, texCoords.y, texCoords.z, texCoords.w), is_flipped);
			m_TexturePath = texPath;
		}

		Animation() = default;
		~Animation() = default;
	};
}