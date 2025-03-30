#pragma once
#include "Window.h"

namespace FE2D {
	class FOR_API Input {
	public:
		Input() = default;
		~Input() = default;

		void Release();
		void Initialize(Window* window);

		void UpdateDirection();

		inline const vec2& getDirection()noexcept { 
			this->UpdateDirection(); 
			return m_Direction;
		}

	private:
		Window* m_Window = nullptr;
	private:
		vec2 m_Direction = vec2();

		std::unordered_map<int, bool> m_KeyStates;
	};
}