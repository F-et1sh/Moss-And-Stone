#pragma once
#include "Window.h"

namespace FE2D {
	class FOR_API Camera {
	public:
		Camera() = default;
		~Camera() = default;
	public:
		void BindToWindow(Window* window);

	public:

		inline const vec2& getVisionSize() {
			this->synchronize();
			return m_VisionSize;
		}

		inline const vec2& getPosition() {
			this->synchronize();
			return m_Position;
		}

		inline void setVisionSize(const vec2& size) {
			m_VisionSize_Reference = nullptr;
			m_VisionSize = size;
		}

		inline void setPosition(const vec2& pos) {
			m_Position_Reference = nullptr;
			m_Position = pos;
		}

	public:
		inline operator const mat4()noexcept {
			this->synchronize();

			mat4 load_matrix = translate(mat4(1.0f), vec3(m_Position, 1.0f));

			load_matrix *= ortho(
				-m_VisionSize.x / 2,
				 m_VisionSize.x / 2,
				-m_VisionSize.y / 2,
				 m_VisionSize.y / 2,
				 -1000.0f,
				  1000.0f
			);

			return load_matrix;
		}

	private:
		// synchronize vision size and position with pointers if they are exist
		inline void synchronize() {
			if (m_VisionSize_Reference)
				m_VisionSize = *m_VisionSize_Reference;

			if (m_Position_Reference)
				m_Position = *m_Position_Reference;
		}

		inline void reset_pointers() {
			m_VisionSize_Reference = nullptr;
			m_Position_Reference = nullptr;
		}

	public:
		vec2 m_VisionSize = vec2();
		vec2 m_Position = vec2();

	private:
		vec2* m_VisionSize_Reference = nullptr;
		vec2* m_Position_Reference = nullptr;
	};
}