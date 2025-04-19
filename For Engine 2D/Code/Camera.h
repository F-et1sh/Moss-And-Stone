#pragma once

namespace FE2D {
	class FOR_API Camera {
	public:
		Camera() = default;
		~Camera() = default;

		inline const mat4& getView()noexcept {
			this->UpdateView();
			return m_View; 
		}

		inline const vec2& getPosition()const noexcept { return m_Position; }
		void setPosition(const vec2& position)noexcept {
			m_Position = position;
			m_IsChanged = true;
		}

	private:
		vec2 m_Position = vec2();

	private:
		void UpdateView();

	private:
		bool m_IsChanged = false;
		mat4 m_View = mat4(1.0f);
	};
}